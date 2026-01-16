/*
 * tuning_store.c
 *
 *  Created on: 15 gen 2026
 *      Author: mdilucchio
 */

#include "tuning/tuning_store.h"

// ====================== Helper ============================
static inline void normalize_minmax(int16_t* mn, int16_t* mx)
{
    if (*mx < *mn) {
        int16_t t = *mn;
        *mn = *mx;
        *mx = t;
    }
}
//===========================================================

void TuningStore_Init(TuningStore* s)
{
    s->duty.valid = false;
    s->current.valid = false;
    s->position.valid = false;
    s->pid0.valid = false;
    s->version = 0u;
}

void TuningStore_ApplySignal(TuningStore* s, uint8_t sub, int16_t minRaw, int16_t maxRaw, uint16_t freq_hz)
{
    normalize_minmax(&minRaw, &maxRaw);

    if (freq_hz > 10000u) freq_hz = 10000u; // guard rail

    TuneSignal* dst = 0;

    /* ATTENZIONE: sub deve matchare TelemetryType Qt */
    switch (sub) {
        case 0: dst = &s->duty; break;
        case 1: dst = &s->current; break;
        case 3: dst = &s->position; break; // nel tuo uart_to_qt: POSITION = 3
        default: return;
    }

    dst->min_raw = minRaw;
    dst->max_raw = maxRaw;
    dst->freq_hz = freq_hz;
    dst->valid   = true;

    s->version++;
}

void TuningStore_ApplyPid(TuningStore* s, uint8_t pidId, int32_t kp_m, int32_t ki_m, int32_t kd_m)
{
    (void)pidId; // serve se quando un domani ci saranno più controllori (X/Y), per ora lo posso ignorare
    s->pid0.kp_milli = kp_m;
    s->pid0.ki_milli = ki_m;
    s->pid0.kd_milli = kd_m;
    s->pid0.valid    = true;
    s->version++;
}

uint32_t TuningStore_Version(const TuningStore* s)
{
    return s->version;
}
