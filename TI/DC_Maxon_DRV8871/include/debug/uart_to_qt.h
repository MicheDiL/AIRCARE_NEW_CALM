/*
 * uart_to_qt.h
 *
 *  Created on: 2 gen 2026
 *      Author: mdilucchio
 *
 *       TX RM57->QT:
 *       Producer (main/control loop): DrvSci_Write(bytes, len) mette i byte in TX ring
 *       Consumer (ISR TX): a fine blocco SCI_TX_INT fa consume e lancia il blocco successivo con sciSend()
 *
 *      13/01/26
 *      uart diventa il protocol layer:
 *      - continua a fare TX telemetria (già ok)
 *      - aggiunge un parser incrementale per i frame Qt (12/18 byte), facendo:
 *      - sync su header 0x55AA (on-wire: AA 55)
 *      - riconoscimento comando (0x10 o 0x20)
 *      - raccolta byte
 *      - CRC XOR
 *      - pubblicazione di “pending updates” (senza bloccare)
 *
 *      Qui la logica: NON fare parsing in ISR. Lo fai nel loop a 1 kHz (o anche 100 Hz), leggendo un budget di byte da RX ring
 *      Quindi RX Qt->RM57:
 *      RX interrupt parcheggia byte in un ring buffer, poi il loop 1kHz drena un budget di byte e li passa al parser byte-by-byte,
 *      che fa sync su magic, capisce quanto è lungo il frame, lo ricostruisce, fa CRC, e se OK alza un flag “pending” che
 *      l’applicazione può consumare quando le torna comodo con UartRxPop;
 */

#ifndef INCLUDE_DEBUG_UART_TO_QT_H_
#define INCLUDE_DEBUG_UART_TO_QT_H_
/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "debug/drv_sci.h"
/********************************************************************************************************************/

/*************************************************** Protocol constants *********************************************/
/* Magic identico al Qt/MCU virtuale:
   kMagic = 0x55AA  -> sul filo (little-endian) bytes: 0xAA 0x55
   Essenziale per:
       - resync nello stream
       - riconoscere in modo robusto dove inizia un frame*/
#define UART_TLM_MAGIC  (0x55AAu)

#define ADD_FUNCTIONS_TO_GUI
#ifdef ADD_FUNCTIONS_TO_GUI
#else
#define NO_FUNCTIONS                // Ongi segnale (Duty/Current/Position) ha sempre gli stessi 3 parametri (min, max, freq)
#endif

/* Type identico al Qt (TelemetryType : quint8)
   IMPORTANTE: i valori numerici devono matchare quelli in protocol.h di Qt
   perchè Qt mappa type->plot specifico */
typedef enum
{
    UART_TLM_DUTY     = 0,   // Duty
    UART_TLM_CURRENT  = 1,   // Current
    UART_TLM_VOLTAGE  = 2,   // Voltage
    UART_TLM_POSITION = 3,   // Position

    TLM_MAX           = 4
} UartTelemetryType;

/********************************************************************************************************************/

/************************************************** TELEMETRIA Qt (TX) *******************************************/
///////////////////////////////////////////////// PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART  ///////////////////////////////////////////
/* Frame da 8B compatibile con SerialWorker::parseBuffer() (Qt).
   Layout:
     hdr   : 16-bit magic 0x55AA
     type  : 8-bit
     id    : 8-bit (0=measure, 1=target)
     value : 16-bit utilizzo la logica del FIXED-POINT per mandare dati float
     crc   : 16-bit XOR simple */
typedef struct __attribute__((packed)) // __attribute__((packed)) serve perché altrimenti il compilatore ARM potrebbe inserire padding
{
    uint16_t hdr;
    uint8_t  type;
    uint8_t  id;
    uint32_t tick_ms;   // <-- NEW
    int16_t  value;
    uint16_t crc;
} UartTelemetryPktWire;

// NEW
_Static_assert(sizeof(UartTelemetryPktWire) == 12, "UartTelemetryPktWire must be 12 bytes");

///////////////////////////////////////////////// SECONDA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART  ///////////////////////////////////////////
/* Nuovo frame “burst” (1 pacchetto per tick)
Obiettivo
Invece di questo (attuale):
    - 8 × UartTelemetryPktWire da 12B = 96B per tick
passi a:
    - 1 × UartTelemetryBurstWire da 24B = 24B per tick
Risultato: meno banda, meno overhead, meno parsing, e soprattutto tick atomico (tutti i canali sincronizzati sullo stesso tick_ms).*/
typedef struct __attribute__((packed))
{
    uint16_t hdr;       // 0x55AA
    uint32_t tick_ms;   // DrvRti_Millis() al momento del tick
    int16_t  v[8];      // 8 valori raw (fixed-point)
    uint16_t crc;       // XOR 16-bit
} UartTelemetryBurstWire;

_Static_assert(sizeof(UartTelemetryBurstWire) == 24, "UartTelemetryBurstWire must be 24 bytes");

/************************************************** RX tuning (Qt -> MCU) *******************************************/
/* I cmd devono matchare con quelli di Qt: il parser usa questi valori per sapere la lunghezza attesa del frame ricevuto */
typedef enum {
    #ifdef NO_FUNCTIONS
        UART_CMD_SIGNAL_TUNING  = 0x10u,      // come CmdType::SignalTuning su Qt
    #endif
    #ifdef ADD_FUNCTIONS_TO_GUI
        UART_CMD_WAFEFORM_TUNING = 0X11u,     // come CmdType su Qt
    #endif
    UART_CMD_PID_TUNING     = 0x20u       // come CmdType::PidTuning su Qt
} UartCmdType;

#ifdef ADD_FUNCTIONS_TO_GUI
    /* Match Qt SignalShape*/
    typedef enum {
        UART_SHAPE_SIN          = 0u,
        UART_SHAPE_STEPTRIANGLE = 1u,
        UART_SHAPE_CONST        = 2u
    } UartSignalShape;
#endif

#ifdef NO_FUNCTIONS
    /* Qt manda questi frame:
        - SignalTuningCmdWire (12 bytes)
        - PidTuningCmdWire (18 bytes)
    Sul RM57 aggiungiamo struct packed equivalenti */
    typedef struct __attribute__((packed)) {
        uint16_t hdr;      // 0x55AA
        uint8_t  cmd;      // 0x10
        uint8_t  sub;      // TelemetryTypeId (Duty/Current/Position)
        int16_t  min;      // fixed-point dipende dal tipo (come Qt)
        int16_t  max;      // fixed-point dipende dal tipo (come Qt)
        uint16_t freq_hz;  // Hz interi
        uint16_t crc;      // XOR 16-bit
    } UartSignalTuningCmdWire;
#endif
#ifdef ADD_FUNCTIONS_TO_GUI
    /* New UartWaveformTuningCmdWire (16B) */
    typedef struct __attribute__((packed)) {
        uint16_t hdr;    // 0x55AA
        uint8_t  cmd;    // 0x11
        uint8_t  sub;    // TelemetryType (Duty/Current/Voltage/Position)
        uint8_t  id;     // 0..1 (Qt usa 1=target)
        uint8_t  shape;  // SignalShape (0/1/2)
        int16_t  min;
        int16_t  max;
        int16_t  aux1;   // StepTraingle: steAmplitude (stepAmp) | Sin/Const = 0
        uint16_t aux2;   // Sin: freq_hz | StepTraingle: stepMs | Const: 0
        uint16_t crc;    // XOR 16-bit
    } UartWaveformTuningCmdWire;
#endif

typedef struct __attribute__((packed)) {
    uint16_t hdr;      // 0x55AA
    uint8_t  cmd;      // 0x20
    uint8_t  sub;      // pidId
    int32_t  kp_milli; // *1000
    int32_t  ki_milli;
    int32_t  kd_milli;
    uint16_t crc;      // XOR 16-bit
} UartPidTuningCmdWire;
//Nota: endianness: su RM57 (ARM little-endian) e su PC Windows (x86 little-endian) l’on-wire è coerente

/* Pending Storage */
#ifdef NO_FUNCTIONS
    typedef struct {
        bool     pending;   // pending significa: è arrivato un comando valido, ma non l’ho ancora consumato nel controllo
        int16_t  min_raw;
        int16_t  max_raw;
        uint16_t freq_hz;
    } UartSigPending;
#endif
#ifdef ADD_FUNCTIONS_TO_GUI
    typedef struct {
        bool     pending;   // pending significa: è arrivato un comando valido, ma non l’ho ancora consumato nel controllo
        uint8_t  id;
        uint8_t  shape;
        int16_t  min_raw;
        int16_t  max_raw;
        int16_t  aux1_raw;
        uint16_t aux2;
    } UartWavePending;
#endif

typedef struct {
    bool     pending;
    uint8_t  pidId;
    int32_t  kp_milli;
    int32_t  ki_milli;
    int32_t  kd_milli;
} UartPidPending;

typedef struct
{
    /* parser state */
    uint8_t  buf[18];   // buffer del frame corrente (per quanto detto in protocol.h su Qt so che un frame di tuning può essere al max 18 byte)
    uint8_t  idx;       // quanti byte nel frame corrente hai già raccolto
    uint8_t  want_len;  // lunghezza attesa (12 o 18) una volta letto il cmd

    /* (opzionali) KPI qualità linea */
    uint32_t crc_err;   // frame con CRC sbagliata
    uint32_t frame_err; // frame malformato

    /* pending: latest commands */
    #ifdef NO_FUNCTIONS
        UartSigPending sig[TLM_MAX];  // ultimo comando ricevuto per ogni segnale 0:Duty 1:Current 2:Voltage 3:Position (match enum UartTelemetryType)
    #endif
    #ifdef ADD_FUNCTIONS_TO_GUI
        UartWavePending wave[TLM_MAX];
    #endif
    UartPidPending pid;     // ultimo PID ricevuto

} UartRxCtx;
/********************************************************************************************************************/

/************************************************* Public API (TX only) ********************************************/
/* Init del modulo logico telemetria:
   - period_ms: ogni quanto (in ms) inviare l’intero burst di telemetria */
void UartTelemetry_Init(uint16_t period_ms);

/* Configura i parametri di rate in runtime (opzionale) */
void UartTelemetry_SetPeriodMs(uint16_t period_ms);

/* Invio di un singolo sample (1 frame da 8B) — non usato di solito in loop,
   ma utile per debug o per invii sporadici */
//bool UartTelemetry_SendSample(UartTelemetryType type, uint8_t id, int16_t value);

/* Invio di una coppia (measure+target) per un tipo:
   - genera 2 frame (16B) e li accoda in un solo write (atomicità del pair) */
//bool UartTelemetry_SendPair(UartTelemetryType type, int16_t meas, int16_t targ);

int16_t uart_encode_value(UartTelemetryType t, float v_phys);

///////////////////////////////////////////////// PRIMA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART  ///////////////////////////////////////////
/* Funzione “tick” da chiamare nel loop di controllo:
   quando è scaduto il periodo, invia 8 frame (64Byte):
     Duty:     id0 measure, id1 target
     Current:  id0 measure, id1 target
     Voltage:  id0 measure, id1 target (se vuoi 2 canali, altrimenti uguali)
     Position: id0 measure, id1 target */
void UartTelemetry_Tick(
        int16_t duty_meas,     int16_t duty_targ,
        int16_t current_meas,  int16_t current_targ,
        int16_t voltage0,      int16_t voltage1,
        int16_t position_meas, int16_t position_targ
);
///////////////////////////////////////////////// SECONDA TIPOLOGIA DI PROTOCOLLO DI STREAMING UART  ///////////////////////////////////////////
void UartTelemetryBurst_Tick(
        int16_t duty_meas,     int16_t duty_targ,
        int16_t current_meas,  int16_t current_targ,
        int16_t voltage0,      int16_t voltage1,
        int16_t position_meas, int16_t position_targ
);
/************************************************* Public API (RX only) ********************************************/
/* init + poll */
void UartRx_Init(UartRxCtx* r);

/* parse e aggiorna pending */
void UartRx_Poll(UartRxCtx* r, uint16_t budget_bytes);

/* Consumer non bloccanti */
#ifdef NO_FUNCTIONS
    bool UartRx_PopSignal(UartRxCtx* r, UartTelemetryType* sub, int16_t* minRaw, int16_t* maxRaw, uint16_t* freq_hz);
#endif
#ifdef ADD_FUNCTIONS_TO_GUI
    bool UartRx_PopWaveform(UartRxCtx* r, UartTelemetryType* sub, uint8_t* id, uint8_t* shape, int16_t* minRaw, int16_t* maxRaw, int16_t* aux1Raw, uint16_t* aux2);
#endif

bool UartRx_PopPid(UartRxCtx* r, uint8_t* pidId, int32_t* kp_milli, int32_t* ki_milli, int32_t* kd_milli);

/* helper decode (opzionali) */
float UartDecode_Duty01(int16_t* raw);      // %*100 -> duty[0..1]
float UartDecode_CurrentA(int16_t* raw);    // mA*10 -> A
float UartDecode_PositionDeg(int16_t* raw); // deg*1000 -> deg
float UartDecode_Gain(int32_t* milli);      // milli -> float
/********************************************************************************************************************/

#endif /* INCLUDE_DEBUG_UART_TO_QT_H_ */
