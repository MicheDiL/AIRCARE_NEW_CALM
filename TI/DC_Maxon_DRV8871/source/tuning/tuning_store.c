/*
 * tuning_store.c
 *
 *  Created on: 15 gen 2026
 *      Author: mdilucchio
 */

#include "tuning/tuning_store.h"
#include <stddef.h>

// ====================== Helper ============================
static inline void normalize_minmax(int16_t* mn, int16_t* mx)
{
    if (*mx < *mn) {
        int16_t t = *mn;
        *mn = *mx;
        *mx = t;
    }
}

static inline TuneWaveform* select_dst(TuningStore* s, uint8_t sub)
{
    /* ATTENZIONE: sub deve matchare TelemetryType Qt:
       Duty=0, Current=1, Voltage=2, Position=3 */
    switch (sub) {
        case 0u: return &s->duty;
        case 1u: return &s->current;
        case 3u: return &s->position;  /* Position = 3 */
        default: return NULL;
    }
}

//===========================================================

void TuningStore_Init(TuningStore* s)
{
    if(!s) return;
        memset(s, 0, sizeof(*s));   // inizializza a 0 tutti i parametri di TuneWaveform per duty, current e position; inizializza a 0 tutti i parametri di TunePid

    s->duty.valid = false;
    s->current.valid = false;
    s->position.valid = false;

    s->pid0.valid = false;
    s->version = 0u;
}


void TuningStore_ApplyWaveform(TuningStore* s,
                               uint8_t sub,
                               uint8_t id,
                               uint8_t shape,
                               int16_t minRaw,
                               int16_t maxRaw,
                               int16_t aux1Raw,
                               uint16_t aux2)
{
    TuneWaveform* dst = select_dst(s, sub);
    if (!dst) return;

    normalize_minmax(&minRaw, &maxRaw);

    /* guard-rail su shape */
    if (shape > (uint8_t)TUNE_SHAPE_CONST) return;

    /* guard-rail su id (per ora accettiamo solo 0..1) */
    if (id > 1u) return;

    /* guard-rail su aux2 per evitare numeri “tossici” */
    if (aux2 > 60000u) aux2 = 60000u;

    if(shape == TUNE_SHAPE_CONST) {
        aux1Raw = 0;
        aux2 = 0;
    }

    dst->id       = id;
    dst->shape    = shape;
    dst->min_raw  = minRaw;
    dst->max_raw  = maxRaw;
    dst->aux1_raw = aux1Raw;
    dst->aux2     = aux2;
    dst->valid    = true;       // tutto corretto e safe

    // attualmente incrementa anche se i valori di tuning che arrivano da Qt sono identici ai precedenti già inviati
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
