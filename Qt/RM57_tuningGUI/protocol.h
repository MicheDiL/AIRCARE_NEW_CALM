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


#define ADD_FUNCTIONS_TO_GUI        // Ongi segnale (Duty/Current/Position) può essere di una forma (Sin/ StepTriangle/ Const) e ogni form a ha parametri diversi

#ifdef ADD_FUNCTIONS_TO_GUI
#else
#define NO_FUNCTIONS                // Ongi segnale (Duty/Current/Position) ha sempre gli stessi 3 parametri (min, max, freq)
#endif

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
    Position = 3    // attualmente misuriamo i V del ADC, in futuro questo dato dovrà essere espresso in °

    //Count
};

// NEW
inline quint16 xor16Of32(quint32 v)
{
    const quint16 lo = quint16(v & 0xFFFFu);
    const quint16 hi = quint16((v >> 16) & 0xFFFFu);
    return quint16(lo ^ hi);
}

///////////////////////////////////////////////// PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART  ///////////////////////////////////////////
#pragma pack(push, 1)
struct TelemetryPktWire
{
    quint16 hdr;        // kMagic
    quint8  type;       // TelemetryType
    quint8  id;         // 0..1 con 0 = measure, 1 = target
    quint32 tick_ms;    // NEW: timestamp RM57
    qint16  value;      // value_raw = round(value_physical * scaleFor(type))
    quint16 crc;        // XOR semplice su hdr, meta(type/id), e value
}; // Quindi un frame contiene un solo valore e “tipo+id” dicono a Qt su quale grafico/curva inserirlo
#pragma pack(pop)

static_assert(sizeof(TelemetryPktWire) == 12, "TelemetryPktWire must be 12 bytes"); // NEW

// controllo di integrità per rumore casuale facendo XOR (^) tra i campi del pacchetto
inline quint16 crcTelemetry(const TelemetryPktWire& p)
{
    const quint16 meta = quint16(p.type) | (quint16(p.id) << 8);  // meta impacchetta i due byte type e id in una word 16-bit
    //return quint16(p.hdr ^ meta ^ quint16(p.value));    // Questo corrisponde alla idea “XOR semplice su word”
    return quint16(p.hdr ^ meta ^ xor16Of32(p.tick_ms) ^ quint16(p.value)); // NEW
}

///////////////////////////////////////////////// SECONDA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART  ///////////////////////////////////////////
#pragma pack(push, 1)
struct TelemetryBurstWire
{
    quint16 hdr;       // kMagic
    quint32 tick_ms;   // RM57 tick
    qint16  v[8];      // 8 raw fixed-point values
    quint16 crc;       // XOR 16-bit
};
#pragma pack(pop)

static_assert(sizeof(TelemetryBurstWire) == 24, "TelemetryBurstWire must be 24 bytes");

inline quint16 crcTelemetryBurst(const TelemetryBurstWire& p)
{
    quint16 c = quint16(p.hdr ^ xor16Of32(p.tick_ms));
    for(int i=0;i<8;i++)
        c = quint16(c ^ quint16(p.v[i]));
    return c;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Conversione Qt-friendly -> Oggetto “comodo” lato Qt per fare il plot (già convertito in double + timestamp)
struct TelemetrySample
{
    double tSec; // = 0.0;
    TelemetryType type; // = TelemetryType::Duty;
    quint8 id; // = 0;
    double value;// = 0.0; // già scalato
};

// Dichiaro i tipi “trasportabili” via signal/slot tra thread
Q_DECLARE_METATYPE(TelemetryType)
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

#ifdef ADD_FUNCTIONS_TO_GUI

/* Ogni tab ha una QComboBox per selezionare la forma.
    Sotto c’è un QStackedWidget con 3 pagine:
    Sin → pannello min/max/freq
    StepTriang → pannello min/max/stepAmp/stepMs
    Const → pannello constValue
    */
enum class SignalShape  : quint8  {
    Sin = 0,
    StepTriangle = 1,
    Const = 2
};

struct SignalConfig  {
    SignalShape  type = SignalShape ::Sin;

    // sin
    double min_sin = 0.0;
    double max_sin = 1.0;
    double freq_sin = 1.0;  // Hz

    // step triangle
    double min_triang = 0.0;
    double max_triang = 1.0;
    double stepAmp = 0.1;   // unità segnale
    int stepMs = 100;       // ms

    // const
    double constValue = 0.0;
};
#endif

inline double scaleFor(TelemetryType t)
{
    switch (t) {
    case TelemetryType::Duty:     return 100.0;   // centi-% (scala*100) -> risoluzione di due decimali e range [-327.68%, +327.67%]  che è ok per il dutycicle [0%,100%]
    case TelemetryType::Current:  return 10.0;    // mA                  -> risoluzione di un decimale  e range [-3276.8mA,+3276.7mA] che è ok per le nostre correnti operative [-2A,2A]
    case TelemetryType::Position: return 1000.0;  // milli-deg (ma per ora è V)           -> risoluzione di tre decimali e range [-32.768°,+32.767°]   che è ok per i limiti del angolo solido di CALM [-8°,8°]
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
    case TelemetryType::Position: return qBound(-10.0,   v, 10.0);      // ° (ma per ora è V)
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
    #ifdef NO_FUNCTIONS
        SignalTuning = 0x10,  // sizeof(SignalTuningCmdWire) == 12
    #endif
    #ifdef ADD_FUNCTIONS_TO_GUI
        WaveformTuning = 0x11,  // nuovo (16B) <-- quello che useremo
    #endif
    PidTuning    = 0x20   // sizeof(PidTuningCmdWire) == 18
};  // Questa è la cosa migliore quando ricevi uno stream UART: trovi AA 55, leggi cmd, sai quanti byte aspettarti, poi fai CRC e applichi.

#ifdef NO_FUNCTIONS
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
#endif

#ifdef ADD_FUNCTIONS_TO_GUI
    #pragma pack(push, 1)
    struct WaveformTuningCmdWire   // 16 bytes
    {
        quint16 hdr;     // 0x55AA
        quint8  cmd;     // 0x11 (CmdType::WaveformTuning)
        quint8  sub;     // TelemetryType (Duty/Current/Voltage/Position)

        quint8  id;      // 0..1 (noi useremo 1 = target): l'altro id potrebbe essere utile se in futuro si vuole decidere da Qui il tipo di risposta da ottenere dal sistema
        quint8  shape;   // SignalShape (Sin/StepTriangle/Const)

        qint16  min;     // scaled (come prima)
        qint16  max;     // scaled

        qint16  aux1;    // Sin: 0 | StepTriangle: stepAmp(scaled) | Const: 0
        quint16 aux2;    // Sin: freq_Hz | StepTriangle: stepMs | Const: 0

        quint16 crc;     // XOR 16-bit
    };
    #pragma pack(pop)

    static_assert(sizeof(WaveformTuningCmdWire) == 16, "WaveformTuningCmdWire must be 16 bytes");

    inline quint16 crcWaveformTuningCmd(const WaveformTuningCmdWire& p)
    {
        const quint16 meta0 = quint16(p.cmd) | (quint16(p.sub)   << 8);
        const quint16 meta1 = quint16(p.id)  | (quint16(p.shape) << 8);

        return quint16(p.hdr ^ meta0 ^ meta1 ^
                       quint16(p.min) ^ quint16(p.max) ^
                       quint16(p.aux1) ^ quint16(p.aux2));
    }

    inline QByteArray makeWaveformTuningCmd(TelemetryType t, quint8 id, const SignalConfig& cfg)
    {
        /*  cfg è la reference alla struct contenente tutti i parametri tunabili
         *  della shape selezionata */

        WaveformTuningCmdWire p{};
        p.hdr   = kMagic;
        p.cmd   = quint8(CmdType::WaveformTuning);  // Tipo di comando => lato MCU puoi leggere cmd e sai esattamente la dimensione del pacchetto
        p.sub   = quint8(t);            // Quale TelemetryType sto configurando
        p.id    = id;                   //  1 = target
        p.shape = quint8(cfg.type);     // Quale forma (Sin/StepTriangle/Const)

        auto setMinMax = [&](double mn, double mx){
            /* Labmda function che prende in input dati presi dal contesto per riferimento ([&])*/
            mn = clampPhysical(t, mn);
            mx = clampPhysical(t, mx);

            if(mx < mn) std::swap(mn, mx);

            p.min = encodeValue(t, mn);
            p.max = encodeValue(t, mx);
        };

        // default aux: per alcune forme non servono => li metti a 0 per convenzione
        p.aux1 = 0;
        p.aux2 = 0;

        // Shape decide come interpretare aux1/aux2
        switch(cfg.type)
        {
        case SignalShape::Sin:
            setMinMax(cfg.min_sin, cfg.max_sin);
            //p.aux2 = cfg.freq_sin;
            {
            const int fHz = qRound(cfg.freq_sin);              // double -> int con arrotondamento
            p.aux2 = quint16(qBound(0, fHz, 60000));
            }

            break;

        case SignalShape::StepTriangle:
            setMinMax(cfg.min_triang, cfg.max_triang);
            p.aux1 = encodeValue(t, cfg.stepAmp);             // step amplitude scalato come il segnale
            p.aux2 = quint16(qBound(0, cfg.stepMs, 60000));   // stepMs in 0..60000 ms (scegli tu)
            break;

        case SignalShape::Const: {
            // puoi scegliere: usare min=max=const (molto comodo sul micro)
            const double v = clampPhysical(t, cfg.constValue);
            p.min = encodeValue(t, v);
            p.max = p.min;
            // aux = 0
        } break;
        }

        p.crc = crcWaveformTuningCmd(p);
        return QByteArray(reinterpret_cast<const char*>(&p), sizeof(p));
    }

#endif

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
