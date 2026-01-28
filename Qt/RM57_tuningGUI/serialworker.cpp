#include "serialworker.h"
#include <cstring> // memcpy

// Facciamo il log dei dati di telemetria per analisi offline
#include <QDateTime>
#include <QDir>

#include <QThread>

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

    // Avvia batching (timer nel thread del worker)
    startBatching();
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
    /* La UART non sa nulla di struct, campi, né di frame.
    Vede solo:
    55 AA 11 xx ... crc
    55 AA 11 xx ... crc
    55 AA 11 xx ... crc
    55 AA 20 xx ... crc
    back-to-back
    Il parser che su RM57 distingue i frame è la funzione feed_byte nel modulo uart_to_qt.c
    */
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
    _flushTimer->setInterval(_flushMs); // es.33ms
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
//                                      SLOT
//====================================================================================================================

void SerialWorker::setLogDirectory(const QString& dir)
{
    const QString newDir = dir.trimmed(); // riceve la nuova directory selezionata dall'utente e rimuove gli spazi (normalizzazione della directory)

    // Se la directory nuova è uguale a quella attuale non fare nulla
    if(newDir == _logDir)
        return;

    // Se stavi registrando, fai “rotate” in modo atomico:
    // stop -> aggiorna base -> nuova session directory -> restart dei REC accesi
    const bool wasRecording = anyRecordingEnabled();

    // salvo per ciascun type lo stato REC
    std::array<bool, std::tuple_size_v<decltype(_rec)>> wasOn{}; // vettore di dimensione fissa derivata da _rec che essendo std::array<T,N> è tuple-like in C++
    if(wasRecording){
        for(size_t  i=0;i < _rec.size();i++) wasOn[i] = _rec[i].enabled;
        stopAllRecording();
    }

    // Applico la nuova base directory e mi assicuro che esista
    _logDir = newDir;
    if(!_logDir.isEmpty())
        QDir().mkpath(_logDir);

    // forza nuova sessione su nuova base
    _sessionDir.clear();

    if(wasRecording){
        // riaccendi i type che erano ON
        for(size_t  i=0;i < _rec.size() ;i++){
            if(wasOn[i])
                setRecording(static_cast<TelemetryType>(i), true);
        }
    }
}

void SerialWorker::setRecording(TelemetryType type, bool enabled)
{
    const int ti = int(type);

    RecSink& r = _rec[ti];

    // DEBUG 0: ingresso slot
    qInfo() << "[REC] setRecording ENTER"
            << "type=" << ti
            << "enabled=" << enabled
            << "_logDir=" << _logDir
            << "_sessionDir(before)=" << _sessionDir
            << "r.enabled(before)=" << r.enabled
            << "r.fileName(before)=" << r.file.fileName();

    if(ti < 0 || ti >= int(_rec.size())){
        qWarning() << "[REC] setRecording: type out of range:" << ti;
        return;
    }

    if(enabled)
    {
        if(r.enabled) {
            qInfo() << "[REC] already enabled -> return";
            return;
        }

        qInfo() << "[REC] enabling -> calling ensureRecDir()";

        ensureRecDir(); // prepare la directory in cui salvare i log

        qInfo() << "[REC] after ensureRecDir _sessionDir=" << _sessionDir;

        // Ora creo il nome del file da inserire nella directory creata da ensureRecDir()
        const QString stamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
        const QString fileName = QString("%1_%2.csv").arg(typeName(type), stamp);
        const QString path = QDir(_sessionDir).filePath(fileName);

        qInfo() << "[REC] computed fileName=" << fileName;
        qInfo() << "[REC] computed path=" << path;

        // PROVA: verifica lo stato reale del QFile membro
        qInfo() << "[REC] r.fileName(pre-set)=" << r.file.fileName();

        r.file.close();                 // safety
        r.file.setFileName(path);

        qInfo() << "[REC] r.fileName(post-set)=" << r.file.fileName();

        const bool openOk = r.file.open(QIODevice::WriteOnly | QIODevice::Text);

        qInfo() << "[REC] openOk=" << openOk
                << "r.fileName(at-open)=" << r.file.fileName()
                << "error=" << r.file.errorString();

        if(!openOk){
            qWarning() << "[REC] OPEN FAILED hard"
                       << "path=" << path
                       << "fileName(in QFile)=" << r.file.fileName();
            r.enabled = false;
            return;
        }

        r.ts.setDevice(&r.file);
        r.ts.setRealNumberNotation(QTextStream::FixedNotation);
        r.ts.setRealNumberPrecision(6);

        r.ts << "tSec,type,id,raw,value\n"; // prima riga del file.csv
        r.lines = 0;
        r.enabled = true;

        qInfo() << "[REC] ENABLED OK"
                << "final fileName=" << r.file.fileName();
        return;
    }
    else
    {
        if(!r.enabled) {
            qInfo() << "[REC] disable requested but already OFF, no-op for type=" << ti;
            return;
        }

        // DEBUG 8: stop
        qInfo() << "[REC] disabling type=" << ti
                << "file=" << r.file.fileName()
                << "lines=" << r.lines;

        r.ts.flush();
        r.file.close();
        r.enabled = false;
        r.lines = 0;

        // opzionale: se tutti OFF, chiudi sessione
        const bool any = anyRecordingEnabled();

        // DEBUG 9: stato post-stop
        qInfo() << "[REC] disabled OK"
                << "type=" << ti
                << "anyRecordingEnabled=" << any
                << "_sessionDir(beforeClear)=" << _sessionDir;

        if(!any){
            _sessionDir.clear();
            qInfo() << "[REC] sessionDir cleared (all REC OFF)";
        }

        return;
    }
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
    /* Scatta ad ogni timeout di _flushTimer (33ms) */

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

    //const int frameSize = int(sizeof(TelemetryPktWire)); // esiste un frame fisso TelemetryPktWire di dimensione sizeof(TelemetryPktWire) (12 byte) // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART
    const int frameSize = int(sizeof(TelemetryBurstWire)); // unico burst da 24B contenente tutti i dati di streaming

    while (_rx.size() >= frameSize) {   // Se hai meno di 8 byte: non puoi fare niente, aspetti il prossimo chunk

        // 1) Cerca header per resync
        const int idx = _rx.indexOf(magic); // trovi il primo punto in cui potrebbe iniziare un pacchetto

        if (idx < 0) {
            // Non trovato: tengo l’ultimo byte (può essere 0xAA e il 0x55 arriva nel prossimo chunk)
            _rx = (_rx.size() > 1) ? _rx.right(1) : QByteArray{};
            return;
        }

        // Scarta spazzatura prima dell’header
        if (idx > 0)
            _rx.remove(0, idx);

        // Se header trovato ma non ho ancora frame completo, aspetto
        if (_rx.size() < frameSize)
            return;

        // 2) Copia i bytes nella struct packed
        //TelemetryPktWire p{}; // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART
        TelemetryBurstWire p{};

        std::memcpy(&p, _rx.constData(), frameSize);    // memcpy = copia byte-per-byte. Qui il presupposto è che TelemetryPktWire sia packed (senza padding). Tipico: #pragma pack(push,1)

        // 3) Validazione: hdr e CRC
        const bool magicOk = (p.hdr == kMagic);
        // ricostruisce il checksum e lo confronta
        //const bool crcOk   = (p.crc == crcTelemetry(p)); // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART
        const bool crcOk   = (p.crc == crcTelemetryBurst(p));

        if (!magicOk || !crcOk) {
            // Resync robusto: resync “sliding” -> sposti di 1 byte e riparti. È la tecnica più robusta in streaming binario
            _rx.remove(0, 1);
            continue;
        }

        // 4) Consumi e converti in TelemetrySample
        _rx.remove(0, frameSize);   // rimuovi il frame dal buffer

        // 5) Crei sample “GUI-friendly”

        //TelemetrySample s{}; // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART

        //s.tSec  = _t0.elapsed() / 1000.0;   // tempo di arrivo/parse su PC, non il tempo di campionamento su RM57, convertito da ms a s
        /* ORIGINE DEL PROBLEMA DI VISUALIZZAZIONE DEI DATI:
         QElapsedTimer::elapsed() ritorna interni in ms => se parseBuffer macina tanti interi in meni di 1ms l'effetto è che tanti campioni
         diversi verranno rappresentati nello stesso istante tSec! La GUI plotta chunck di dati per volta:
         Qt emette readyRead per andare a leggere i dati arrivati e poi purseBuffer svuota una valanga di frame in un loop stretto.
         Loop stretto significa periodo di tempo << 1ms => per tutti quei frame elapsed() ritorna lo stesso valore!
         In DeviceDialog::appendTelemtryBatch() uso s.tSec come chiave X dei grafici quindi se per 60 campioni s.tSec è identico,
         stai aggiungendo 60 punti con stessa X e Y diversa!
         In più, tu fai scorrere l’asse X con il “tempo continuo” in double DeviceDialog::tDisplayNow() const ma, dentro questa
         funzione _lastT viene aggiornato con l'ultimo s.tSec da appendTelemtryBacth. Quindi Se _lastT resta “fermo” perché i sample hanno
         stessa tSec, lo scorrimento X non rappresenta più il tempo reale di acquisizione
        */
        // NEW: SOLUZIONE AL PROBLEMA
        //s.tSec  = double(p.tick_ms) / 1000.0;              // <-- TEMPO RM57 convertito da ms->s   // PRIMA TIPOLOGIA DI STREAMING UART


        //s.type  = static_cast<TelemetryType>(p.type);   // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART
        //s.id    = p.id; // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART

        // conversione raw -> unità fisiche coerenti con scaleFor()
        //s.value = decodeValue(s.type, p.value); // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART

        //emit telemetryReceived(s);  // emesso nel thread del worker, ricevuto in GUI con queued connection → aggiornare QCustomPlot è safe
        /* Se la telemetria è ad alta frequenza, emit telemetryReceived per ogni frame può saturare la event queue del GUI thread.
            Pattern consigliato:
                - accumuli samples in una coda nel worker
                - mandi batch ogni X ms (es. 20–50ms) o ogni N samples
        Così il plot resta fluido.
        */

        // --- DUTY LOGGING (first 1000 samples) ---
        //maybeLogFrame(s.tSec, s.type, s.id, p.value, s.value); // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART

        // --- end logging ---

        //_pending.push_back(s); // accumula sample da far plottare al thread GUI // PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART

        ////////////////////////////// SECONDA TIPOLOGIA DI STREAMING UART ///////////////////////////////////////

        // PARSING DI 1 BURST IN 8 TELEMETRYSAMPLE (UNO PER CURVA)
        const double tSec = double(p.tick_ms) / 1000.0; // timestamp RM57 convertito da ms->s

        // 5) Crei sample “GUI-friendly”
        auto push = [&](TelemetryType type, quint8 id, qint16 raw)
        {
            TelemetrySample s{};
            s.tSec  = tSec;
            s.type  = type;
            s.id    = id;
            s.value = decodeValue(type, raw);

            // logging
            maybeLogFrame(s.tSec, s.type, s.id, raw, s.value);

            _pending.push_back(s);  // accumula samples da far plottare al thread GUI che farà flush batch ogni 33ms o soglia N
        };

        // mapping fisso (deve matchare RM57):
        // 0 Duty meas, 1 Duty targ, 2 Cur meas, 3 Cur targ, 4 Volt0, 5 Volt1, 6 Pos meas, 7 Pos targ
        push(TelemetryType::Duty,     0, p.v[0]);
        push(TelemetryType::Duty,     1, p.v[1]);

        push(TelemetryType::Current,  0, p.v[2]);
        push(TelemetryType::Current,  1, p.v[3]);

        push(TelemetryType::Voltage,  0, p.v[4]);
        push(TelemetryType::Voltage,  1, p.v[5]);

        push(TelemetryType::Position, 0, p.v[6]);
        push(TelemetryType::Position, 1, p.v[7]);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Il batch di pacchetti TelemetrySample accumulati in _pending viene spedito verso GUI o dal timer _flushTimer (ogni 33ms) o se il numero di campioni accumulati supera la soglia _flushMaxSamples
        if(_pending.size() >= _flushMaxSamples){
            flushNow();   // flush immediato per evitare accumulo eccessivo
        }
    }
}

// Facciamo il log dei dati di telemetria per analisi offline
void SerialWorker::ensureRecDir()
{
    // se sessione già creata , non ricreare
    if(!_sessionDir.isEmpty())
        return;

    // Base: se GUI ha impostato _logDir, usala. Altrimenti fallback “safe”.
    QString base = _logDir;
    if(base.isEmpty()) {
        const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        base = QDir(docs).filePath("CALM_Logs");
    }

    // Timestamp della sessione
    const QString stamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    // Costruisci il path della sessione fatto da: base (directiry dove voglio mettere i log) + stamp (timestamp)
    _sessionDir = QDir(base).filePath(stamp);

    // Crea fisicamente la directory
    const bool ok = QDir().mkpath(_sessionDir);

    qInfo() << "ensureRecDir base=" << base
            << "session=" << _sessionDir
            << "mkpath=" << ok;

    // (opzionale ma utile) se mkpath fallisce, logga e lascia _sessionDir vuota così non fai casino
    if(!ok) {
        qWarning() << "ensureRecDir FAILED, cannot create directory:" << _sessionDir;
        _sessionDir.clear();
        return;
    }
}


QString SerialWorker::typeName(TelemetryType t) const
{
    switch(t){
    case TelemetryType::Duty:     return "Duty";
    case TelemetryType::Current:  return "Current";
    case TelemetryType::Voltage:  return "Voltage";
    case TelemetryType::Position: return "Position";
    }
    return "Unknown";
}

void SerialWorker::maybeLogFrame(double tSec, TelemetryType type, quint8 id, qint16 raw, double value)
{
    const int ti = int(type);
    if(ti < 0 || ti >= int(_rec.size())) return;

    RecSink& r = _rec[ti];
    if(!r.enabled) return;

    r.ts << tSec << "," << ti << "," << int(id) << "," << int(raw) << "," << value << "\n";
    if((++r.lines % 200) == 0)  // ogni 200 righe fai flush → i dati vengono “spinti” su disco
        r.ts.flush();

    /*Significato colonne:

        tSec: tempo sul PC (non sul RM57), in secondi
        type: numerico (0 Duty, 1 Current, 2 Voltage, 3 Position)
        id: 0 measure, 1 target
        raw: valore int16 arrivato dal frame
        value: valore scalato in double (decodeValue)
    */
}


bool SerialWorker::anyRecordingEnabled() const
{
    for(const auto& r : _rec)
        if(r.enabled) return true;
    return false;
}

void SerialWorker::stopAllRecording()
{
    for(auto& r : _rec) {
        if(!r.enabled) continue;
        r.ts.flush();
        r.file.close();
        r.enabled = false;
        r.lines = 0;
    }
}


