/* protocol.h è il posto in cui metti tutto ciò che riguarda il “wire format” (formato dei pacchetti sul filo UART):

- magic/header e lunghezze
- enum dei tipi di telemetria
- struct packed che rappresentano i pacchetti binari
- calcolo CRC/XOR
- funzioni helper per creare pacchetti di tuning (Qt → MCU)
- tipi “comodi” per l’UI (TelemetrySample) separati dai “wire packet”

Così: se un giorno modifichi TelemetryPkt sul MCU, aggiorni quasi solo questo file e il parser del worker

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Controllo di integrità della trasmissione UART:
- XOR semplice può essere sufficiente come “sanity check” + resync
- la cosa più importante è avere header + lunghezza fissa + resync (come facciamo nel parsing)

Se però vuoi farlo “industriale” e più resistente a disturbi reali (cavi lunghi, EMI, ecc.), passare a CRC-16 polinomiale è un salto di qualità
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Telemetria MCU → Qt:
frame fisso 8 byte (TelemetryPktWire) con CRC XOR semplice: 4 tipi Duty / Current / Voltage / Position, con id 0..1 (2 canali per tipo).
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tuning Qt → MCU:
Signal tuning (SignalTuningCmdWire 12 byte: min/max/freq) solo per Duty/Current/Position (Voltage non tunabile)
PID tuning (PidTuningCmdWire 18 byte : Kp/Ki/Kd) solo comando, nessuna telemetria PID
*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtGlobal>                                     // tipi Qt a dimensione fissa (quint16, qint16, ecc.)
#include <QMetaType>                                    // serve per poter spedire un tuo tipo (TelemetrySample) dentro signals/slots cross-thread
#include <QByteArray>                                   // pacchetti binari come “array di byte”
#include <QElapsedTimer>
#include <QVector>
/*
    NOTE ENDIANNESS:
    - hdr è uint16. Se MCU e PC sono little-endian, inviando kMagic=0x55AA
      sulla linea vedrai bytes: AA 55.
*/

/* Questo magic serve per:
- resync robusto nello stream RX
- guard-rail contro rumore/spazzatura*/
static constexpr quint16 kMagic = 0x55AA;

// ==============================================================================================
//                                      MCU -> Qt (Telemetry Data)
// ==============================================================================================

/* OSS:
1) #pragma pack() serve perchè stai dicendo al compilatore di “non mettere padding” fra i campi della struct

    quindi:
    - la struct in memoria avrà esattamente i byte consecutivi come sul filo UART

    Perché è fondamentale?

    - perché nel worker fai memcpy dallo stream a questa struct.
    - se ci fosse padding, sizeof(struct) sarebbe diverso e i campi non sarebbero allineati.

2) static_assert(sizeof.....) è una “cintura di sicurezza”:

    - se un domani qualcuno cambia un tipo (int al posto di qint16, ecc.)
    - oppure se il packing non venisse applicato

la compilazione fallisce invece di farti debuggare un bug misterioso.
*/
enum class TelemetryType : quint8 { // Tipi di dati plottabili su devicedialog
    Duty     = 0,   // %
    Current  = 1,   // mA
    Voltage  = 2,   // V
    Position = 3    // unità "pos"
};


//============== BEGIN: NUOVE MODIFICHE =============
enum class DutyWaveform : int {
    Sin = 0,
    StepTriangle = 1,
    Const = 2
};

struct DutyCfg {
    DutyWaveform type = DutyWaveform::Sin;

    // sin
    double min = 0.0;
    double max = 1.0;
    double freq = 1.0;

    // step triangle
    double stepAmp = 0.1;
    int stepMs = 100;

    // const
    double constValue = 0.0;
};

// ============= END: NUOVE MODIFICHE ==============

#pragma pack(push, 1)
struct TelemetryPktWire
{
    quint16 hdr;    // kMagic
    quint8  type;   // TelemetryType
    quint8  id;     // 0..1 con 0 = measure, 1 = target
    qint16  value;  // value_centi (valore * 100)
    quint16 crc;    // XOR semplice su hdr, meta(type/id), e value
}; // Quindi un frame contiene un solo valore e “tipo+id” dicono a Qt su quale grafico/curva inserirlo
#pragma pack(pop)

static_assert(sizeof(TelemetryPktWire) == 8, "TelemetryPktWire must be 8 bytes");

// controllo di integrità per rumore casuale facendo XOR (^) tra i campi del pacchetto
inline quint16 crcTelemetry(const TelemetryPktWire& p)
{
    const quint16 meta = quint16(p.type) | (quint16(p.id) << 8);  // meta impacchetta i due byte type e id in una word 16-bit
    return quint16(p.hdr ^ meta ^ quint16(p.value));    // Questo corrisponde alla idea “XOR semplice su word”
}

// Conversione Qt-friendly -> Oggetto “comodo” lato Qt (già convertito in double + timestamp)
struct TelemetrySample
{
    double tSec = 0.0;
    TelemetryType type = TelemetryType::Duty;
    quint8 id = 0;
    double value = 0.0; // già scalato (value_centi/100.0)
};

// Rendo i tipi “trasportabili” via signal/slot tra thread
Q_DECLARE_METATYPE(TelemetrySample) // Questo dice a Qt che “TelemetrySample è un tipo che può viaggiare nel sistema metaobject” e serve perchè emettiamo un segnale telemetryReceived da un thread e lo riceviamo in un altro con queued connection
Q_DECLARE_METATYPE(QVector<TelemetrySample>) // Idem per il tipo QVector<TelemetrySample> in cui accumulo i campioni da far plottare al thread GUI

// ==============================================================================================
//                                      Qt -> MCU (Tuning Commands)
// ==============================================================================================
/* Anche per i pacchetti di tuning utilizziamo lo stesso header usato per i pacchetti di telemetria.
 Inoltre sia per SignalTuning che per PidTuning adottiamo una strategia di codifica fixed-point tale per cui i
 numeri reali vengono rappresentati come interi scalati, così sul filo (UART) e sul micro non hai
 byte float/double.
    Ex:
    SignalTuning: metodo di codifica "scala*100" significa che un valore come -12.34 viene spedito sul cavo UART come -1234(qint16)
                 Quando MCU riceve deve dividere il valore ricevuto per 100.
                 Dato che qint16 va da -32768 a +32767 allora con scala*100 il range rappresentabile è -327.68...+327.67
 In generale:
 Encoding (PC→MCU):
    raw = round(value_real * SCALE)
 Decoding:
    value_real = raw / SCALE
*/


inline double scaleFor(TelemetryType t)
{
    switch (t) {
    case TelemetryType::Duty:     return 100.0;   // centi-% (scala*100) -> risoluzione di due decimali e range [-327.68%, +327.67%]  che è ok per il dutycicle [0%,100%]
    case TelemetryType::Current:  return 10.0;    // mA                  -> risoluzione di un decimale  e range [-3276.8mA,+3276.7mA] che è ok per le nostre correnti operative [-2A,2A]
    case TelemetryType::Position: return 1000.0;  // milli-deg           -> risoluzione di tre decimali e range [-32.768°,+32.767°]   che è ok per i limiti del angolo solido di CALM [-8°,8°]
    case TelemetryType::Voltage:  return 1000.0; // esempio: mV (se ti serve)
    }
    return 1.0;
}

// clamp “fisico” per tipo (in unità reali, double)
inline double clampPhysical(TelemetryType t, double v)
{
    switch (t) {
    case TelemetryType::Duty:     return qBound(0.0,    v, 100.0);      // %
    case TelemetryType::Current:  return qBound(-2000.0, v, 2000.0);    // mA
    case TelemetryType::Position: return qBound(-10.0,   v, 10.0);      // °
    case TelemetryType::Voltage:  return v; // dipende dal tuo range reale
    }
    return v;
}

inline qint16 encodeValue(TelemetryType t, double v)
{
    v = clampPhysical(t, v);
    const double s = scaleFor(t);

    const qint32 raw = qRound(v * s);
    const qint32 clamped = qBound(qint32(-32768), raw, qint32(32767));
    return qint16(clamped);
}

inline double decodeValue(TelemetryType t, qint16 raw)
{
    const double s = scaleFor(t);
    return double(raw) / s;
}

enum class CmdType : quint8 {   // tipi di dati da tunare su MCU
    SignalTuning = 0x10,  // sizeof(SignalTuningCmdWire) == 12
    PidTuning    = 0x20   // sizeof(PidTuningCmdWire) == 18
};  // Questa è la cosa migliore quando ricevi uno stream UART: trovi AA 55, leggi cmd, sai quanti byte aspettarti, poi fai CRC e applichi.

// ---- Command: Signal tuning (12 bytes) ----
// cmd = SignalTuning
// sub = TelemetryType (Duty/Current/Position)
// p1  = min
// p2  = max
// p3  = freq_hz (NOTA: qui Hz intero, perché i tuoi spinbox freq hanno decimals=0)
#pragma pack(push, 1)
struct SignalTuningCmdWire // (12byte)
{
    quint16 hdr;      // 2B  = 0x55AA  -> AA 55 on-wire
    quint8  cmd;      // 1B  = 0x10 (CmdType::SignalTuning)
    quint8  sub;      // 1B  = TelemetryType (Duty/Current/Voltage/Position)
    qint16  min;      // 2B  = min (signed)
    qint16  max;      // 2B  = max (signed)
    quint16 freq_hz;  // 2B  = Hz interi (unsigned)
    quint16 crc;      // 2B  = XOR semplice su 16 bit
};
#pragma pack(pop)

static_assert(sizeof(SignalTuningCmdWire) == 12, "SignalTuningCmdWire must be 12 bytes");

// controllo di integrità per rumore casuale facendo XOR (^) tra i campi del pacchetto
inline quint16 crcSignalTuningCmd(const SignalTuningCmdWire& p)
{
    const quint16 meta = quint16(p.cmd) | (quint16(p.sub) << 8); // compattiamo type e id in una word da 16 bit
    // fai XOR tra tre parole da 16 bit, tutte dello stesso formato
    return quint16(p.hdr ^ meta ^ quint16(p.min) ^ quint16(p.max) ^ quint16(p.freq_hz));
}

inline QByteArray makeSignalTuningCmd(TelemetryType t, double minVal, double maxVal, double freqHz)
{
    // clamp e ordine
    minVal = clampPhysical(t, minVal);
    maxVal = clampPhysical(t, maxVal);
    if (maxVal < minVal) std::swap(minVal, maxVal);

    SignalTuningCmdWire p{};
    p.hdr = kMagic;
    p.cmd = quint8(CmdType::SignalTuning);
    p.sub = quint8(t);  // quale segnale tunare

    // min/max nel fixed-point del tipo t
    const double s = scaleFor(t);
    const qint32 minRaw = qRound(minVal * s);
    const qint32 maxRaw = qRound(maxVal * s);

    p.min = qint16(qBound(-32768, minRaw, 32767));
    p.max = qint16(qBound(-32768, maxRaw, 32767));

    // freq: intero Hz, clamp 0..65535
    const int f = qRound(freqHz);
    p.freq_hz = quint16(qBound(0, f, 10000));

    p.crc = crcSignalTuningCmd(p);   // checksum

    // lo trasformi in QByteArray
    return QByteArray(reinterpret_cast<const char*>(&p), sizeof(p));
}


// ---- Command: PID tuning (18 bytes) ----
// cmd = PidTuning
// sub = pidId (0..1) (se un domani hai più controllori)
// kp/ki/kd sono qint32 in milli (x1000) -> ex: kx = 1.234 => kx_milli = round(1.234 * 1000) = round(1234) = 1234
// usiamo qint32 perchè per i guadagni potremmo aver bisogno di range più ampi di quelli di SignalTuningCmd
#pragma pack(push, 1)
struct PidTuningCmdWire // (18 byte)
{
    quint16 hdr;      // 2B  = 0x55AA
    quint8  cmd;      // 1B  = 0x20 (CmdType::PidTuning)
    quint8  sub;      // 1B  = pidId (0..255)
    qint32  kp_milli; // 4B  = kp * 1000
    qint32  ki_milli; // 4B
    qint32  kd_milli; // 4B
    quint16 crc;      // 2B (XOR su 16 bit)
};
#pragma pack(pop)

static_assert(sizeof(PidTuningCmdWire) == 18, "PidTuningCmdWire must be 18 bytes");

inline quint16 xor16Of32(qint32 v)
{
    /* Dato che Kp/ki/kd sono 32-bit, per fare XOR su 16 bit bisogna ridurre ogni
     32-bit in una "word" a 16-bit */
    const quint32 u = quint32(v);
    const quint16 lo = quint16(u & 0xFFFF);         // prende i 16 bit bassi
    const quint16 hi = quint16((u >> 16) & 0xFFFF); // prende i 16 bit alti
    return quint16(lo ^ hi);                        // fa la XOR per ottenere una “firma” a 16 bit del valore 32-bit
}

// controllo di integrità per rumore casuale facendo XOR (^) tra i campi del pacchetto
inline quint16 crcPidTuningCmd(const PidTuningCmdWire& p)
{
    const quint16 meta = quint16(p.cmd) | (quint16(p.sub) << 8);
    return quint16(p.hdr ^ meta ^
                   xor16Of32(p.kp_milli) ^
                   xor16Of32(p.ki_milli) ^
                   xor16Of32(p.kd_milli));
}

inline QByteArray makePidTuningCmd(quint8 pidId, double kp, double ki, double kd)
{
    PidTuningCmdWire p{};
    p.hdr = kMagic;
    p.cmd = quint8(CmdType::PidTuning);
    p.sub = pidId;

    p.kp_milli = qint32(qRound(kp * 1000.0));
    p.ki_milli = qint32(qRound(ki * 1000.0));
    p.kd_milli = qint32(qRound(kd * 1000.0));

    p.crc = crcPidTuningCmd(p);

    return QByteArray(reinterpret_cast<const char*>(&p), sizeof(p));
}


#endif // PROTOCOL_H
