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
#include <math.h>

/* Timer base (ms) dal tuo RTI */
extern uint32_t DrvRti_Millis(void);

/* =========================================================================================
 *                                  TX  (MCU -> Qt)
 * ========================================================================================= */

/* -------------------- Helpers -------------------- */
static inline int16_t clamp_i16(int32_t x)
{
    if (x >  32767) return  32767;
    if (x < -32768) return -32768;
    return (int16_t)x;
}

static inline uint16_t xor16_of_u32(uint32_t v)
{
    uint16_t lo = (uint16_t)(v & 0xFFFFu);
    uint16_t hi = (uint16_t)((v >> 16) & 0xFFFFu);
    return (uint16_t)(lo ^ hi);
}

static inline uint16_t crcBurst(const UartTelemetryBurstWire* p)
{
    uint16_t c = (uint16_t)(p->hdr ^ xor16_of_u32(p->tick_ms));
    int i;
    for (i = 0; i < 8; ++i)
        c = (uint16_t)(c ^ (uint16_t)p->v[i]);
    return c;
}

/* -------------------- */

/* UART 8N1 (1 start + 8 data + 1 stop) ogni byte “costa” 10 bit sul filo. */
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

/* Encode in fixed-point coerente con protocol.h di Qt */
int16_t uart_encode_value(UartTelemetryType t, float v_phys)
{
    /* assume che v_phys sia già espresso nelle unità “fisiche” che Qt vuole vedere
     * dopo la divisione (Duty in %, Current in mA, Voltage in V, Position in deg). */
    float s = 1.0f;
    switch (t) {
    case UART_TLM_DUTY:     s = 100.0f;  break;   // [%] *100
    case UART_TLM_CURRENT:  s = 10.0f;   break;   // [mA]*10
    case UART_TLM_VOLTAGE:  s = 1000.0f; break;   // [V]*1000
    case UART_TLM_POSITION: s = 1000.0f; break;   // [deg]*1000
    default: s = 1.0f; break;
    }

    /* arrotonda e clampa in int16 */
    int32_t raw = (int32_t)lroundf(v_phys * s); // raw è int32 perché la moltiplicazione e il rounding possono produrre valori fuori dal range di int16
    return clamp_i16(raw);  // garantisce che il valore trasmesso sia sempre rappresentabile nel campo  int16
}

/* =============================== Frame builder TX ===================================== */
void UartTelemetryBurst_Tick(                 // 1 burst da 24B contenente i dati di 8 frame (2 type * 4 plot)
        int16_t duty_meas,     int16_t duty_targ,
        int16_t current_meas,  int16_t current_targ,
        int16_t voltage0,      int16_t voltage1,
        int16_t position_meas, int16_t position_targ
)
{
    const uint32_t now = DrvRti_Millis();
    if (now < s_next_ms) return;

    UartTelemetryBurstWire b;
    b.hdr     = (uint16_t)UART_TLM_MAGIC;
    b.tick_ms = now;    // timestamp uguale per tutti i frame dello stesso burst

    b.v[0] = duty_meas;
    b.v[1] = duty_targ;
    b.v[2] = current_meas;
    b.v[3] = current_targ;
    b.v[4] = voltage0;
    b.v[5] = voltage1;
    b.v[6] = position_meas;
    b.v[7] = position_targ;

    b.crc = 0u;
    b.crc = crcBurst(&b);

    (void)DrvSci_Write((const uint8_t*)&b, (uint16_t)sizeof(b));

    s_next_ms = now + (uint32_t)s_period_ms;
}

/* =========================================================================================
 *                                  RX tuning (Qt -> MCU)
 * ========================================================================================= */
/* -------------------- Helpers -------------------- */

/* ---- CRC identiche a Qt ----
 *  Signal: XOR su hdr, meta(cmd/sub), min, max, freq
    PID: XOR su hdr, meta + XOR16(kp) + XOR16(ki) + XOR16(kd)*/
static inline uint16_t crcWaveformCmd(const UartWaveformTuningCmdWire* p)
{
    const uint16_t meta0 = (uint16_t)p->cmd | ((uint16_t)p->sub << 8);
    const uint16_t meta1 = (uint16_t)p->id | ((uint16_t)p->shape << 8);
    return (uint16_t)(p->hdr ^ meta0 ^ meta1 ^
            (uint16_t)p->min ^ (uint16_t)p->max ^
            (uint16_t)p->aux1 ^ (uint16_t)p->aux2);
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

/* -------------------- */

// Assert fondamentali perchè se èer qualunque motivo il compilatore introducesse padding, me ne accorgerei subuto in compile-time
_Static_assert(sizeof(UartWaveformTuningCmdWire) == 16, "UartWaveformTuningCmdWire must be 16 bytes");
_Static_assert(sizeof(UartPidTuningCmdWire)    == 18, "UartPidTuningCmdWire must be 18 bytes");

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
    /* Quando hai un frame completo in r->buf[], fai:
        - memcpy in una struct packed
        - controlli hdr == 0x55AA
        - controlli crc
    se ok, aggiorni pending
    */
    if (r->want_len == (uint8_t)sizeof(UartWaveformTuningCmdWire)) {

        UartWaveformTuningCmdWire p;
        memcpy(&p, r->buf, sizeof(p));

        if (p.hdr != UART_TLM_MAGIC) { r->frame_err++; return; }
        if (p.crc != crcWaveformCmd(&p)) { r->crc_err++; return; }

        // sub identifica quale segnale (Duty/Current/Voltage/Position)
        if (p.sub < (uint8_t)TLM_MAX) {
            // salvi i parametri “raw” (fixed-point)
            r->wave[p.sub].id       = p.id;
            r->wave[p.sub].shape    = p.shape;
            r->wave[p.sub].min_raw  = p.min;
            r->wave[p.sub].max_raw  = p.max;
            r->wave[p.sub].aux1_raw  = p.aux1;
            r->wave[p.sub].aux2     = p.aux2;
            r->wave[p.sub].pending  = true; // metti pending=true (cioè “ho un update nuovo pronto da consumare”)
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
    // Magic 0x55AA -> on-wire little-endian: AA 55
    const uint8_t M0 = 0xAAu;
    const uint8_t M1 = 0x55u;

    /* ---------------- Sync sul magic (AA 55) ---------------- */
    // aspetta primo byte del magic (0xAA)
    if (r->idx == 0u)
    {
        if (b == M0) {
            r->buf[0] = b;
            r->idx = 1u;
        }
        return;
    }

    // aspetta secondo byte del magic (0x55)
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

    /* ---------------- Leggo cmd e decido la lunghezza (idx=2) ---------------- */
    if (r->idx == 2u)
    {
        r->buf[2] = b; // ora il byte ricevuto è cmd

    // Quindi una volta letto cmd sai esattamente quanti byte devi raccogliere per avere un frame completo
    if (b == (uint8_t)UART_CMD_WAFEFORM_TUNING){
        r->want_len = (uint8_t)sizeof(UartWaveformTuningCmdWire);
    } else if (b == (uint8_t)UART_CMD_PID_TUNING) {
        r->want_len = (uint8_t)sizeof(UartPidTuningCmdWire);
    } else {
        // cmd sconosciuto -> resync
        r->frame_err++;
        r->idx = 0u;
        r->want_len = 0u;
        return;
    }

        r->idx = 3u;
        return;
    }

    /* ---------------- Raccolgo i byte fino a want_len ---------------- */
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
        handle_frame(r);    // Validazione frame e “pending updates”
        r->idx = 0u;
        r->want_len = 0u;
    }
}

void UartRx_Poll(UartRxCtx* r, uint16_t budget_bytes)
{
    uint8_t tmp[32]; // scelta comoda poichè serve solo a drenare dal ring RX in chunk piccoli e comodi, senza lavorare a byte singolo e senza usare troppo stack

    // legge dal ring RX “a chunk” (max 32 byte per volta)
    while (budget_bytes > 0u) {
        uint16_t chunk = (budget_bytes > (uint16_t)sizeof(tmp)) ? (uint16_t)sizeof(tmp) : budget_bytes;
        uint16_t n = DrvSci_Read(tmp, chunk);
        if (n == 0u) break;

        uint16_t i;
        for (i = 0; i < n; ++i) {
            // passa ogni byte ricevuto dal ring RX al parser incrementale
            feed_byte(r, tmp[i]);
        }
        budget_bytes = (uint16_t)(budget_bytes - n);
    }
}

bool UartRx_PopWaveform(UartRxCtx* r, UartTelemetryType* sub, uint8_t* id, uint8_t* shape, int16_t* minRaw,
                        int16_t* maxRaw, int16_t* aux1Raw, uint16_t* aux2)
{
    /* scorre wave[0..3] e ritorna il primo che ha pending=true */
    uint8_t i;
    for (i = 0u; i < TLM_MAX; ++i) {
        if (r->wave[i].pending) {
            // Quando lo trova:
            r->wave[i].pending = false; // lo mette a pending=false

            // copia i campi in output param
            if (sub)     *sub       = (UartTelemetryType)i;
            if (id)      *id        = r->wave[i].id;
            if (shape)   *shape     = r->wave[i].shape;

            if (minRaw)  *minRaw    = r->wave[i].min_raw;
            if (maxRaw)  *maxRaw    = r->wave[i].max_raw;

            if (aux1Raw) *aux1Raw   = r->wave[i].aux1_raw;
            if(aux2)     *aux2      = r->wave[i].aux2;

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
} // decodifica un valore di duty espresso come %*100 su Qt in un valore fra 0 e 1

float UartDecode_CurrentA(int16_t* raw){
    return ((float)(*raw)) / 10000.0f;
} // mA*10 -> A

float UartDecode_PositionDeg(int16_t* raw) {
    return ((float)(*raw)) / 1000.0f;
} // deg*1000 -> deg

float UartDecode_Gain(int32_t* milli){
    return ((float)(*milli)) / 1000.0f;
}

