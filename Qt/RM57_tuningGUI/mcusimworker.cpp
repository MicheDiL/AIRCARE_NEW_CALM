#ifdef MCU_SIM

#define _USE_MATH_DEFINES
#include "mcusimworker.h"

#include <cmath>
#include <cstring>

McuSimWorker::McuSimWorker(QObject* parent)
    : QObject(parent)
{

}

McuSimWorker::~McuSimWorker()
{
    stopSim();
}

void McuSimWorker::resetPort()
{
    if(_txTimer){
        _txTimer->stop();
        _txTimer->deleteLater();
        _txTimer = nullptr;
    }

    if(_sp){
        if(_sp->isOpen()) _sp->close();
        _sp->deleteLater();
        _sp = nullptr;
    }

    _rx.clear();
}

void McuSimWorker::startSim(const QString& portName, int baud)
{
    resetPort();

    // Quando parte la simulazione apre COM2 con QSerialPort
    _sp = new QSerialPort(this);
    _sp->setPortName(portName);
    _sp->setBaudRate(baud);
    _sp->setDataBits(QSerialPort::Data8);
    _sp->setParity(QSerialPort::NoParity);
    _sp->setStopBits(QSerialPort::OneStop);

    connect(_sp, &QSerialPort::readyRead, this, &McuSimWorker::onReadyRead);

    if(!_sp->open(QIODevice::ReadWrite)){
        emit simStatus(false, "Sim open failed: " + _sp->errorString());
        resetPort();
        return;
    }

    // IMPORTANTISSIMO: crea QTimer ora (sei nel thread del worker sim)
    _txTimer = new QTimer(this);
    // Rate adattivo: a 9600 non puoi sparare 8 frame a 50Hz (saturi la linea)
    _txIntervalMs = (baud <= 9600) ? 120 : _txIntervalMs;
    _txTimer->setInterval(_txIntervalMs);

    // QTimer emette timeout periodicamente → chiama onTxTick()
    // Quindi immediatamente dopo Connect, il simulatore inizia a chiamare onTxTick() ogni _txIntervalMs
    connect(_txTimer, &QTimer::timeout, this, &McuSimWorker::onTxTick);

    _t0.restart();
    _txTimer->start();

    emit simStatus(true, QString("Sim running on %1 @%2 (tx=%3ms)")
                             .arg(portName).arg(baud).arg(_txIntervalMs));
}

void McuSimWorker::stopSim()
{
    resetPort();
    emit simStatus(false, "Sim stopped");
}

void McuSimWorker::onReadyRead()
{
    if(!_sp || !_sp->isOpen()) return;
    _rx += _sp->readAll();
    parseCmdBuffer();
}

void McuSimWorker::applySignalTuning(const SignalTuningCmdWire& c)
{
    const auto t = static_cast<TelemetryType>(c.sub);

    const double minV = c.min_centi / 100.0;
    const double maxV = c.max_centi / 100.0;
    const double fHz  = double(c.freq_hz);

    auto apply = [&](SigCfg& cfg){
        cfg.min = minV;
        cfg.max = maxV;
        cfg.freqHz = (fHz < 0.0 ? 0.0 : fHz);
        if(cfg.max < cfg.min) std::swap(cfg.min, cfg.max);
    };

    switch(t){
    case TelemetryType::Duty:     apply(_duty); break;
    case TelemetryType::Current:  apply(_cur);  break;
    case TelemetryType::Position: apply(_pos);  break;
    case TelemetryType::Voltage:  /* non tunabile*/ break;
    }
}

void McuSimWorker::parseCmdBuffer()
{
    /* cerchi magic AA 55
     * leggi il byte cmd per capire che struttura aspettarti
     * fai memcpy nella struct corretta (SignalTuningCmdWire o PidTuningCmdWire)
     * validi CRC
     * applichi tuning
    */

    static const QByteArray magic("\xAA\x55", 2);

    while(_rx.size() >= int(sizeof(SignalTuningCmdWire))){

        int idx = _rx.indexOf(magic);
        if(idx < 0){
            _rx = (_rx.size() > 1) ? _rx.right(1) : QByteArray{};
            return;
        }
        if(idx > 0) _rx.remove(0, idx);

        if(_rx.size() < 4) return;

        const quint8 cmd = quint8(_rx[2]);

        int frameSize = 0;
        if(cmd == quint8(CmdType::SignalTuning)) frameSize = int(sizeof(SignalTuningCmdWire));
        else if(cmd == quint8(CmdType::PidTuning)) frameSize = int(sizeof(PidTuningCmdWire));
        else { _rx.remove(0,1); continue; }

        if(_rx.size() < frameSize) return;

        if(frameSize == int(sizeof(SignalTuningCmdWire))){
            SignalTuningCmdWire c{};
            std::memcpy(&c, _rx.constData(), frameSize);

            const bool ok = (c.hdr == kMagic) && (c.crc == crcSignalTuningCmd(c));
            if(!ok){ _rx.remove(0,1); continue; }

            _rx.remove(0, frameSize);
            applySignalTuning(c);   // aggiorna i parametri delle sinusoidi (min/max/freqHz), quindi al tick successivo il simulatore genera nuovi valori coerenti
        }else{
            PidTuningCmdWire c{};
            std::memcpy(&c, _rx.constData(), frameSize);

            const bool ok = (c.hdr == kMagic) && (c.crc == crcPidTuningCmd(c));
            if(!ok){ _rx.remove(0,1); continue; }

            _rx.remove(0, frameSize);
            // PID ignorato nel simulatore (come richiesto)
        }
    }
}

double McuSimWorker::synth(const SigCfg& cfg, double t, double phase)
{
    // Genera una sinusoide centrata su off, ampiezza amp, frequenza freqHz e sfasamento phase
    const double amp = 0.5 * (cfg.max - cfg.min);
    const double off = 0.5 * (cfg.max + cfg.min);
    const double w = 2.0 * M_PI * cfg.freqHz;
    return off + amp * std::sin(w * t + phase);
}

void McuSimWorker::onTxTick()
{

    //Cosa invia il simulatore a ogni tick (cioè cosa finisce nei plot)

    if(!_sp || !_sp->isOpen()) return;

    const double t = _t0.elapsed() / 1000.0;    // ms/1000 = s dove _t0.elapsed() ritorna millisecondi trascorsi (tipo intero) dal restart()

    // Qui crei i valori e li impacchetti in TelemetryPktWire: generi TARGET = ID1 cambiando solo la fase (0 e π/2) e quindi ottieni due sinusoidi sfasate
    // TARGET = ID1 (tunabile)
    const double dutyTarget = synth(_duty, t, 0.0); // _duty è una struct SigCfg con min/max/freqHz, 0.0 è la fase (nessuno sfasamento) altrimenti M_PI/2.0
    const double curTarget  = synth(_cur,  t, 0.0);
    const double posTarget  = synth(_pos,  t, 0.0);

    // MEASURE = ID0 (costante nel simulatore. Sarebbe la misura reale lato RM57)
    const double dutyMeas = 10.0;   // scegli la costante che vuoi
    const double curMeas  = 0.0;
    const double posMeas  = 0.0;

    // Voltage: costante (sia measure che target per coerenza)
    const double vMeas = _voltConst;
    const double vTarget = _voltConst;

    // Costruzione dei pacchetti con ID diversi -> utilizzo una lambda function che ritorna una struct
    /* - auto makePkt = ... : “auto” deduce il tipo della lambda.
     * - [&] : cattura per riferimento tutte le variabili esterne usate dentro (es. kMagic, anche se è globale, e qualunque variabile locale se servisse).
     * In pratica: puoi usare roba esterna senza copiarla. (TelemetryType type, quint8 id, double value) : parametri della lambda.
     * - ->TelemetryPktWire : tipo di ritorno esplicito (utile perché ritorni una struct).*/
    auto makePkt = [&](TelemetryType type, quint8 id, double value)->TelemetryPktWire{
        TelemetryPktWire p{};
        p.hdr = kMagic;
        p.type = quint8(type);
        p.id = id;
        p.value = qint16(qRound(value * 100.0));  // *100.0 → converti un valore “reale” in “centi-unità”
        p.crc = crcTelemetry(p);
        return p;
    };

    const TelemetryPktWire frames[] = {
        makePkt(TelemetryType::Duty,     0, dutyMeas),
        makePkt(TelemetryType::Duty,     1, dutyTarget),

        makePkt(TelemetryType::Current,  0, curMeas),
        makePkt(TelemetryType::Current,  1, curTarget),

        makePkt(TelemetryType::Voltage,  0, vMeas),
        makePkt(TelemetryType::Voltage,  1, vTarget),

        makePkt(TelemetryType::Position, 0, posMeas),
        makePkt(TelemetryType::Position, 1, posTarget),
    };

    // Scrivo sulla seriale
    QByteArray out;
    out.reserve(int(sizeof(frames)));   // prealloca memoria per evitare riallocazioni durante append

    for(const auto& p : frames) // Costruisci 8 frame e li appendi in QByteArray
        out.append(reinterpret_cast<const char*>(&p), sizeof(p));

    _sp->write(out);    // scrive tutti i frame in un colpo
}

#endif // MCU_SIM

