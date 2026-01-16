#ifndef MCUSIMWORKER_H
#define MCUSIMWORKER_H

#ifdef MCU_SIM

#include <QObject>
#include <QString>       // IMPORTANTISSIMO per signals/slots con QString
#include <QSerialPort>
#include <QTimer>
#include <QElapsedTimer>

#include "protocol.h"

class McuSimWorker : public QObject
{
    Q_OBJECT

public:
    explicit McuSimWorker(QObject* parent=nullptr);
    ~McuSimWorker();

public slots:
    void startSim(const QString& portName, int baud);
    void stopSim();

signals:
    void simStatus(bool ok, const QString& reason);

private slots:
    void onReadyRead();
    void onTxTick();

private:
    void resetPort();
    void parseCmdBuffer();
    void applySignalTuning(const SignalTuningCmdWire& c);

    struct SigCfg {
        double min = 0.0;
        double max = 1.0;
        double freqHz = 1.0;
    };

    static double synth(const SigCfg& cfg, double t, double phase);

private:
    QSerialPort* _sp = nullptr;
    QByteArray _rx;

    QTimer* _txTimer = nullptr;
    QElapsedTimer _t0;

    SigCfg _duty  {  0.0, 100.0, 0.5 };
    SigCfg _cur   { -5.0,   5.0, 0.8 };
    SigCfg _pos   { -1.0,   1.0, 0.3 };
    double _voltConst = 2.0;

    /* Ogni frame TelemetryPktWire è 8 byte. Tu mandi 8 frame per tick (duty0/1, cur0/1, volt0/1, pos0/1) ⇒ 64 byte per tick.

        A 115200 baud:
            - 115200 bit/s ≈ 14400 byte/s (considerando 10 bit per byte: start+8+stop)
            - tick a 50 Hz (20ms) ⇒ 64*50 = 3200 byte/s ok :)
            - tick a 200 Hz (5ms)⇒ 64*200 = 12800 byte/s al limite ma ancora ok :|
            - tick a 500 Hz ⇒ 64*500 = 32000 byte/s impossibile :(
        Quindi a 115200 puoi scendere tranquillamente fino a ~5 ms (200 Hz) ma già sei vicino al limite. Per sicurezza: 10–20 ms è ottimo*/
    int _txIntervalMs = 10; // 100 Hz
};

#endif // MCU_SIM
#endif // MCUSIMWORKER_H
