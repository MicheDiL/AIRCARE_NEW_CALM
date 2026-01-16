/*
 * uart_to_qt.c
 *
 *  Created on: 2 gen 2026
 *      Author: mdilucchio
 *
 *      Modulo che prepara i frame da plottare sulla GUI Qt e riceve i comandi di tuning da Qt.
 *      Questo modulo è correlato ai moduli protocol.h e serialworker dell'applicativo Qt.
 */
#include "debug/uart_to_qt.h"
#include <string.h>

/* Timer base (ms) dal tuo RTI */
extern uint32_t DrvRti_Millis(void);

/* =============================== CRC telemetria Qt =================================
   Nel tuo Qt (protocol.h) la CRC è XOR “semplice” sui campi.
   Qui la facciamo su 16-bit words:

     crc = hdr ^ (type | id<<8) ^ value

   - hdr è già 16-bit
   - type+id compattati in 16-bit
   - value è int16 -> cast a uint16 per XOR
*/

// controllo di integrità per rumore casuale facendo XOR (^) tra i campi del pacchetto
static inline uint16_t uart_crcTelemetry(const UartTelemetryPktWire* p)
{
    const uint16_t w_type_id = (uint16_t)p->type | ((uint16_t)p->id << 8); // compattiamo type e id in una word da 16 bit
    // fai XOR tra tre parole da 16 bit, tutte dello stesso formato
    return (uint16_t)(p->hdr ^ w_type_id ^ (uint16_t)p->value);
}

/* =============================== Scheduler TX ========================================= */
static uint16_t s_period_ms = 20u; // 50 Hz
static uint32_t s_next_ms   = 0u;

void UartTelemetry_Init(uint16_t period_ms)
{
    s_period_ms = (period_ms == 0u) ? 1u : period_ms;
    s_next_ms   = 0u;
}

void UartTelemetry_SetPeriodMs(uint16_t period_ms)
{
    s_period_ms = (period_ms == 0u) ? 1u : period_ms;
}

/* =============================== Frame builder TX ===================================== */
static inline UartTelemetryPktWire makeFrame(UartTelemetryType type, uint8_t id, int16_t value)
{
    UartTelemetryPktWire p;
    p.hdr   = (uint16_t)UART_TLM_MAGIC;
    p.type  = (uint8_t)type;
    p.id    = id;
    p.value = value;
    p.crc   = 0u;
    p.crc   = uart_crcTelemetry(&p);
    return p;
}

// Singolo Sample
bool UartTelemetry_SendSample(UartTelemetryType type, uint8_t id, int16_t value)
{
    UartTelemetryPktWire p = makeFrame(type, id, value);
    return DrvSci_Write((const uint8_t*)&p, (uint16_t)sizeof(p));
}

// Coppia measure+target
bool UartTelemetry_SendPair(UartTelemetryType type, int16_t meas, int16_t targ)
{
    /* Burst da 16B: atomicità del pair (o entrano entrambi nel ring o nessuno) */
    UartTelemetryPktWire frames[2];
    frames[0] = makeFrame(type, 0u, meas);  // id0 = measure
    frames[1] = makeFrame(type, 1u, targ);  // id1 = target

    return DrvSci_Write((const uint8_t*)frames, (uint16_t)sizeof(frames));
}

// Burst completo da 8 frmae (64Byte)
void UartTelemetry_Tick(
        int16_t duty_meas,     int16_t duty_targ,
        int16_t current_meas,  int16_t current_targ,
        int16_t voltage0,      int16_t voltage1,
        int16_t position_meas, int16_t position_targ
)
{
    const uint32_t now = DrvRti_Millis();   // decide se è tempo di inviare
    if (now < s_next_ms) return;

    /* 8 frame = 64 bytes.
       Li mando in un’unica enqueue così:
       - non rischio di droppare a metà un “gruppo”
       - riduco overhead (1 sola chiamata DrvSci_Write)
    */
    UartTelemetryPktWire frames[8]; // array di struct

    /* Duty: measure/target */
    frames[0] = makeFrame(UART_TLM_DUTY,     0u, duty_meas); // id0 = measure
    frames[1] = makeFrame(UART_TLM_DUTY,     1u, duty_targ); // id1 = target

    /* Current: measure/target */
    frames[2] = makeFrame(UART_TLM_CURRENT,  0u, current_meas); // id0 = measure
    frames[3] = makeFrame(UART_TLM_CURRENT,  1u, current_targ); // id1 = target

    /* Voltage: id0/id1 (due canali oppure stesso valore duplicato) */
    frames[4] = makeFrame(UART_TLM_VOLTAGE,  0u, voltage0); // id0 = measure
    frames[5] = makeFrame(UART_TLM_VOLTAGE,  1u, voltage1); // id1 = target

    /* Position: measure/target */
    frames[6] = makeFrame(UART_TLM_POSITION, 0u, position_meas); // id0 = measure
    frames[7] = makeFrame(UART_TLM_POSITION, 1u, position_targ); // id1 = target

    // invii 8 frame (64B) in un colpo
    (void)DrvSci_Write((const uint8_t*)frames, (uint16_t)sizeof(frames));

    s_next_ms = now + (uint32_t)s_period_ms;
}


/* =========================================================================================
 *                                  RX tuning (Qt -> MCU)
 * ========================================================================================= */
// Assert fondamentali perchè se èer qualunque motivo il compilatore introducesse padding, me ne accorgerei subuto in compile-time
_Static_assert(sizeof(UartSignalTuningCmdWire) == 12, "UartSignalTuningCmdWire must be 12 bytes");
_Static_assert(sizeof(UartPidTuningCmdWire)    == 18, "UartPidTuningCmdWire must be 18 bytes");

/* ---- CRC identiche a Qt ----
 *  Signal: XOR su hdr, meta(cmd/sub), min, max, freq
    PID: XOR su hdr, meta + XOR16(kp) + XOR16(ki) + XOR16(kd)*/
static inline uint16_t crcSignalCmd(const UartSignalTuningCmdWire* p)
{
    const uint16_t meta = (uint16_t)p->cmd | ((uint16_t)p->sub << 8);
    return (uint16_t)(p->hdr ^ meta ^ (uint16_t)p->min ^ (uint16_t)p->max ^ (uint16_t)p->freq_hz);
}

static inline uint16_t xor16Of32(int32_t v)
{
    const uint32_t u = (uint32_t)v;
    const uint16_t lo = (uint16_t)(u & 0xFFFFu);
    const uint16_t hi = (uint16_t)((u >> 16) & 0xFFFFu);
    return (uint16_t)(lo ^ hi);
}

static inline uint16_t crcPidCmd(const UartPidTuningCmdWire* p)
{
    const uint16_t meta = (uint16_t)p->cmd | ((uint16_t)p->sub << 8);
    return (uint16_t)(p->hdr ^ meta ^
                      xor16Of32(p->kp_milli) ^
                      xor16Of32(p->ki_milli) ^
                      xor16Of32(p->kd_milli));
}

void UartRx_Init(UartRxCtx* r)
{
    /* azzeri tutto: buffer, contatori, pending, KPI
     * riparti in stato "aspetto magic" */
    memset(r, 0, sizeof(*r));
    r->idx = 0u;
    r->want_len = 0u;
}

/* validazione e aggiornamento “pending” */
static void handle_frame(UartRxCtx* r)
{
    if (r->want_len == (uint8_t)sizeof(UartSignalTuningCmdWire)) {
        UartSignalTuningCmdWire p;
        memcpy(&p, r->buf, sizeof(p));

        if (p.hdr != UART_TLM_MAGIC) { r->frame_err++; return; }
        if (p.crc != crcSignalCmd(&p)) { r->crc_err++; return; }

        if (p.sub < 4u) {
            r->sig[p.sub].min_raw  = p.min;
            r->sig[p.sub].max_raw  = p.max;
            r->sig[p.sub].freq_hz  = p.freq_hz;
            r->sig[p.sub].pending  = true;
        } else {
            r->frame_err++;
        }
        return;
    }

    if (r->want_len == (uint8_t)sizeof(UartPidTuningCmdWire)) {
        UartPidTuningCmdWire p;
        memcpy(&p, r->buf, sizeof(p));

        if (p.hdr != UART_TLM_MAGIC) { r->frame_err++; return; }
        if (p.crc != crcPidCmd(&p)) { r->crc_err++; return; }

        r->pid.pidId    = (uint8_t)p.sub; //p.sub;
        r->pid.kp_milli = p.kp_milli;
        r->pid.ki_milli = p.ki_milli;
        r->pid.kd_milli = p.kd_milli;
        r->pid.pending  = true;
        return;
    }

    r->frame_err++;
}

/* state machine parser byte-by-byte */
static void feed_byte(UartRxCtx* r, uint8_t b)
{
    /* Magic 0x55AA -> on-wire little-endian: AA 55 */
    const uint8_t M0 = 0xAAu;
    const uint8_t M1 = 0x55u;

    // Stato 0: aspetta primo byte del magic (0xAA)
    if (r->idx == 0u)
    {
        if (b == M0) {
            r->buf[0] = b;
            r->idx = 1u;
        }
        return;
    }

    // Stato 1: aspetta secondo byte del magic (0x55)
    if (r->idx == 1u)
    {
        if (b == M1) {
            r->buf[1] = b;
            r->idx = 2u;
        } else {
            /* fallback: se arriva di nuovo AA resta armato e non perde sync*/
            r->idx = (b == M0) ? 1u : 0u;
            if (r->idx == 1u) r->buf[0] = b;
        }
        return;
    }

    // Stato 2: byte del cmd (decide lunghezza)
    if (r->idx == 2u)
    {
        /* cmd */
        r->buf[2] = b;

        if (b == (uint8_t)UART_CMD_SIGNAL_TUNING) r->want_len = (uint8_t)sizeof(UartSignalTuningCmdWire); // se cmd==0x10 -> want_len=12 (SignalTuning)
        else if (b == (uint8_t)UART_CMD_PID_TUNING) r->want_len = (uint8_t)sizeof(UartPidTuningCmdWire); // se cmd==0x20 -> want_len=18 (PidTuning)
        else {
            // cmd sconosciuto -> resync
            r->frame_err++;
            r->idx = 0u;
            r->want_len = 0u;
            return;
        }

        r->idx = 3u;
        return;
    }

    /* collecting payload */
    if (r->idx < (uint8_t)sizeof(r->buf))
    {
        r->buf[r->idx++] = b;   // mette ogni byte in buf[idx++]
    } else {
        /* overflow safety (non dovrebbe mai succedere con want_len <= 18) */
        r->frame_err++;
        r->idx = 0u;
        r->want_len = 0u;
        return;
    }

    if (r->want_len != 0u && r->idx >= r->want_len) // quando idx >= want_len -> chiama handle_frame() e resetta idx=0
    {
        handle_frame(r);
        r->idx = 0u;
        r->want_len = 0u;
    }
}

void UartRx_Poll(UartRxCtx* r, uint16_t budget_bytes)
{
    /* Legge dal ring RX massimo budget_bytes per tick e ogni byte passa in feed_byte*/
    uint8_t tmp[32];

    while (budget_bytes > 0u) {
        uint16_t chunk = (budget_bytes > (uint16_t)sizeof(tmp)) ? (uint16_t)sizeof(tmp) : budget_bytes;
        uint16_t n = DrvSci_Read(tmp, chunk);
        if (n == 0u) break;

        uint16_t i;
        for (i = 0; i < n; ++i) {
            feed_byte(r, tmp[i]);
        }
        budget_bytes = (uint16_t)(budget_bytes - n);
    }
}

bool UartRx_PopSignal(UartRxCtx* r, UartTelemetryType* sub, int16_t* minRaw, int16_t* maxRaw, uint16_t* freq_hz)
{
    uint8_t i;
    for (i = 0u; i < TLM_MAX; ++i) {
        if (r->sig[i].pending) {
            r->sig[i].pending = false;
            if (sub)     *sub = (UartTelemetryType)i;
            if (minRaw)  *minRaw = r->sig[i].min_raw;
            if (maxRaw)  *maxRaw = r->sig[i].max_raw;
            if (freq_hz) *freq_hz = r->sig[i].freq_hz;
            return true;
        }
    }
    return false;
}

bool UartRx_PopPid(UartRxCtx* r, uint8_t* pidId, int32_t* kp_milli, int32_t* ki_milli, int32_t* kd_milli)
{
    if (!r->pid.pending) return false;

    r->pid.pending = false;
    if (pidId) *pidId = r->pid.pidId;
    if (kp_milli)  *kp_milli  = r->pid.kp_milli;
    if (ki_milli)  *ki_milli  = r->pid.ki_milli;
    if (kd_milli)  *kd_milli  = r->pid.kd_milli;
    return true;
}

/* helper decode (stesso contratto Qt) */
float UartDecode_Duty01(int16_t* raw){
    return ((float)(*raw)) / 10000.0f;
} // %*100 -> 0..1

float UartDecode_CurrentA(int16_t* raw){
    return ((float)(*raw)) / 10000.0f;
} // mA*10 -> A

float UartDecode_PositionDeg(int16_t* raw) {
    return ((float)(*raw)) / 1000.0f;
} // deg*1000 -> deg

float UartDecode_Gain(int32_t* milli){
    return ((float)(*milli)) / 1000.0f;
}

