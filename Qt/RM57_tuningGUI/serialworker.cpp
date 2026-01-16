#include "serialworker.h"

#include <cstring> // memcpy

SerialWorker::SerialWorker(QObject* parent)
    : QObject(parent)
{

}

SerialWorker::~SerialWorker()
{
    resetPort();
}

//=================================================================================
//                                  SLOT
//=================================================================================
void SerialWorker::openPort(const QString& portName, int baud)
{
    // slot chiamata dalla GUI (queued) → eseguita nel thread del worker

    resetPort();    // reset dello stato precedente (porta + buffer)

    // crea e configura QSerialPort
    _sp = new QSerialPort(this);
    _sp->setPortName(portName);
    _sp->setBaudRate(baud);
    _sp->setDataBits(QSerialPort::Data8);
    _sp->setParity(QSerialPort::NoParity);
    _sp->setStopBits(QSerialPort::OneStop);

    // QSerialPort emette readyRead quando arrivano bytes
    connect(_sp, &QSerialPort::readyRead, this, &SerialWorker::onReadyRead);    // collega readyRead → onReadyRead (RX event-driven)

    // Gestione errori runtime della seriale: così se stacchi USB o cade la COM, la GUI viene notificata
    connect(_sp, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError e){
        if(e == QSerialPort::ResourceError || e == QSerialPort::DeviceNotFoundError){
            emit connectedChanged(false, _sp ? _sp->errorString() : "Serial error");
            resetPort();
        }
    });

    // prova ad aprire in ReadWrite
    if (!_sp->open(QIODevice::ReadWrite)) {
        emit connectedChanged(false, _sp->errorString());   // Se fallisce: emetti un segnale verso la GUI
        resetPort();
        return;
    }

    _t0.restart();

    // emette connectedChanged(true/false, reason)
    emit connectedChanged(true, {});

    #ifndef MCU_SIM_RUN
    // Avvia batching (timer nel thread del worker)
    startBatching();
    #endif
}

void SerialWorker::closePort()
{
    flushNow();      // opzionale: invia ultimi sample rimasti

    // resetta porta e buffer
    resetPort();
    emit connectedChanged(false, "Closed");
}

void SerialWorker::writeBytes(const QByteArray& data)
{
    // guard-rail: scrive solo se porta esiste e aperta
    if (!_sp || !_sp->isOpen())
        return;

    _sp->write(data); // non bloccante: mette i bytes in un buffer interno di QSerialPort e chiede al OS di inviarli senza aspettare che i byte escano davvero dal cavo
}

void SerialWorker::onReadyRead()
{
    // Qt emette readyRead quando arrivano byte nel buffer interno di QSerialPort
    if (!_sp || !_sp->isOpen())
        return;

    _rx += _sp->readAll();  // prende quello che c’è ora nel buffer di QSerialPort -> _rx è il tuo buffer applicativo
    // Attenzione: in UART:
    // un pacchetto può arrivare spezzato
    // oppure possono arrivare più pacchetti insieme
    parseBuffer();          // prova ad estrarre 0..N frame completi
}

void SerialWorker::startBatching()
{   
    if(_flushTimer) return;

    _flushTimer = new QTimer(this);
    _flushTimer->setInterval(_flushMs);
    _flushTimer->setSingleShot(false);

    connect(_flushTimer, &QTimer::timeout, this, &SerialWorker::flushNow);

    _flushTimer->start();
}

void SerialWorker::stopBatching()
{
    if(!_flushTimer) return;
    _flushTimer->stop();
    _flushTimer->deleteLater();
    _flushTimer = nullptr;
    _pending.clear();
}

//====================================================================================================================
//                                      METODI
//====================================================================================================================
void SerialWorker::resetPort()
{
    stopBatching();   // ferma timer e svuota _pending

    if (_sp) {  // se _sp esiste
        if (_sp->isOpen())
            _sp->close();

        _sp->deleteLater();   // distruzione sicura nel thread del worker
        _sp = nullptr;        // eviti uso accidentale dopo la chiusura
    }

    _rx.clear();
}

void SerialWorker::flushNow()
{
    if(_pending.isEmpty())
        return;

    QVector<TelemetrySample> batch;
    batch.swap(_pending);          // O(1) swap: niente copie pesanti

    emit telemetryBatch(batch);    // 1 solo evento queued verso GUI (queued perchè thread diversi)
}

void SerialWorker::parseBuffer()
{
    /* Assunzioni del protocollo RX (implicite nel codice)
    - esiste un frame fisso TelemetryPktWire di dimensione sizeof(TelemetryPktWire) (commento: 8 byte)
    - inizia con magic header kMagic=0x55AA che sul filo è AA 55 (endianness)
    - include un CRC verificabile con crcTelemetry(p)
    - contiene campi: hdr, type, id, value, crc (dedotto dall’uso)
    */

    // Magic bytes di kMagic=0x55AA => in little-endian AA 55
    static const QByteArray magic("\xAA\x55", 2);
    const int frameSize = int(sizeof(TelemetryPktWire)); // esiste un frame fisso TelemetryPktWire di dimensione sizeof(TelemetryPktWire) (8 byte)

    while (_rx.size() >= frameSize) {   // Se hai meno di 8 byte: non puoi fare niente, aspetti il prossimo chunk

        // 1) Cerca header per resync
        const int idx = _rx.indexOf(magic); // trovi il primo punto in cui potrebbe iniziare un pacchetto

        if (idx < 0) {
            // Non trovato: tengo l’ultimo byte (può essere 0xAA e il 0x55 arriva nel prossimo chunk)
            if (_rx.size() > 1)
                _rx = _rx.right(1);
            else
                _rx.clear();
            return;
        }

        // Scarta spazzatura prima dell’header
        if (idx > 0)
            _rx.remove(0, idx);

        // Se header trovato ma non ho ancora frame completo, aspetto
        if (_rx.size() < frameSize)
            return;

        // 2) Copia i bytes nella struct packed
        TelemetryPktWire p{};
        std::memcpy(&p, _rx.constData(), frameSize);    // memcpy = copia byte-per-byte. Qui il presupposto è che TelemetryPktWire sia packed (senza padding). Tipico: #pragma pack(push,1)

        // 3) Validazione: hdr e CRC
        const bool magicOk = (p.hdr == kMagic);
        const bool crcOk   = (p.crc == crcTelemetry(p)); // ricostruisce il checksum e lo confronta
        if (!magicOk || !crcOk) {
            // Resync robusto: resync “sliding” -> sposti di 1 byte e riparti. È la tecnica più robusta in streaming binario
            _rx.remove(0, 1);
            continue;
        }

        // 4) Consumi e converti in TelemetrySample
        _rx.remove(0, frameSize);   // rimuovi il frame dal buffer

        // 5) Crei sample “GUI-friendly”
        TelemetrySample s{};
        s.tSec  = _t0.elapsed() / 1000.0;   // timestamp relativo PC-side
        s.type  = static_cast<TelemetryType>(p.type);
        s.id    = p.id;

        #if MCU_SIM_RUN
        s.value = p.value / 100.0; // value è "centi" sul filo nel caso di MCU simulato
        #else
        // Se MCU non simulato -> scaling per tipo
        switch (s.type) {
        case TelemetryType::Duty:
            s.value = p.value;          // duty% on wire
            break;
        case TelemetryType::Current:
            s.value = p.value;          // mA on-wire (NO /100)
            break;
        case TelemetryType::Voltage:
            s.value = p.value;          // V on-wire
            break;
        case TelemetryType::Position:
            s.value = p.value / 100.0;   // centi-deg
            break;
        default:
            s.value = p.value;
            break;
        }
        #endif

        //emit telemetryReceived(s);  // emesso nel thread del worker, ricevuto in GUI con queued connection → aggiornare QCustomPlot è safe
        /* Se la telemetria è ad alta frequenza, emit telemetryReceived per ogni frame può saturare la event queue del GUI thread.
            Pattern consigliato:
                - accumuli samples in una coda nel worker
                - mandi batch ogni X ms (es. 20–50ms) o ogni N samples
        Così il plot resta fluido.
        */
        _pending.push_back(s); // accumula sample da far plottare al thread GUI

        if(_pending.size() >= _flushMaxSamples){
            flushNow();   // flush immediato per evitare accumulo eccessivo
        }
    }
}
