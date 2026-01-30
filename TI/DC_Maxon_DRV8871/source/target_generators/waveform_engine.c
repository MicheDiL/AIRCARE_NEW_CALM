/*
 * waveform_engine.c
 *
 *  Created on: 29 gen 2026
 *      Author: mdilucchio
 */

#include "target_generators/waveform_engine.h"
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ================= decode raw -> fisico (coerente con Qt scaleFor) ================= */
static float decode_phys(WaveGenSignalType t, int16_t raw)
{
    switch (t) {
    case WGEN_T_DUTY:     return ((float)raw) / 10000.0f;
    case WGEN_T_CURRENT:  return ((float)raw) / 10.0f;    // mA*10 -> mA
    case WGEN_T_POSITION: return ((float)raw) / 1000.0f;  // deg*1000 -> deg
    default:              return (float)raw;
    }
}

/* clamp float */
static float clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static void normalize_minmax_f(float* mn, float* mx)
{
    if (*mx < *mn) { float t = *mn; *mn = *mx; *mx = t; }
}

/* calcolo parametri derivati per sin/step */
static void recompute_cached(WaveGen* g)
{
    /* normalizza min/max */
    normalize_minmax_f(&g->cfg_active.min_v, &g->cfg_active.max_v);

    /* SIN: omega */
    if (g->cfg_active.shape == WGEN_SIN) {
        float f = g->cfg_active.freq_hz;
        if (f < 0.0f) f = 0.0f;
        g->cfg_active.freq_hz = f;
        g->omega = 2.0f * (float)M_PI * f;
    }

    /* STEP: step_ticks */
    if (g->cfg_active.shape == WGEN_STEPTRIANGLE) {
        uint16_t ms = g->cfg_active.step_ms;
        if (ms == 0u) ms = 1u;
        g->cfg_active.step_ms = ms;

        /* step_ticks = ms * fs / 1000 */
        float ticks_f = ((float)ms) * g->fs / 1000.0f;
        uint32_t ticks = (uint32_t)(ticks_f + 0.5f);
        if (ticks == 0u) ticks = 1u;
        g->step_ticks = ticks;
    }

    /* CONST: y può essere riallineato a min/max */
    if (g->cfg_active.shape == WGEN_CONST) {
        /* convenzione: const = min_v (che dovrebbe essere == max_v) */
        g->y = g->cfg_active.min_v;
    }
}

/* =============================== API ================================= */

void WaveGen_Init(WaveGen* g, WaveGenSignalType type, float sample_rate_hz)
{
    memset(g, 0, sizeof(*g));
    g->type = type;
    g->fs = (sample_rate_hz <= 1.0f) ? 10000.0f : sample_rate_hz;
    g->dt = 1.0f / g->fs;

    /* default config */
    g->cfg_active.shape = WGEN_CONST;
    g->cfg_active.min_v = 0.0f;
    g->cfg_active.max_v = 0.0f;
    g->cfg_active.freq_hz = 1.0f;
    g->cfg_active.step_amp = 0.1f;
    g->cfg_active.step_ms = 100u;

    g->y = 0.0f;
    g->phase = 0.0f;
    g->dir = +1;
    g->step_cnt = 0;

    recompute_cached(g);
}

void WaveGen_StageFromTune(WaveGen* g, const TuneWaveform* w)
{
    if (!g || !w || !w->valid) return;

    WaveGenCfg c; // creo un oggetto relativo ai parametri di configurazione per il generatore di target
    memset(&c, 0, sizeof(c));

    /* shape */
    if (w->shape > (uint8_t)WGEN_CONST) return;
    c.shape = (WaveGenShape)w->shape;

    /* min/max */
    c.min_v = decode_phys(g->type, w->min_raw);
    c.max_v = decode_phys(g->type, w->max_raw);
    normalize_minmax_f(&c.min_v, &c.max_v);

    /* interpreta aux1_raw/aux2 in base alla shape */
    if (c.shape == WGEN_SIN) {
        /* Qt: aux2 = freq_hz (intero) */
        c.freq_hz   = (float)w->aux2;
        c.step_amp  = 0.0f;
        c.step_ms   = 0u;
    }
    else if (c.shape == WGEN_STEPTRIANGLE) {
        /* Qt: aux1_raw = stepAmp scalato come segnale, aux2 = stepMs */
        c.step_amp = decode_phys(g->type, w->aux1_raw);
        c.step_ms  = w->aux2;
        c.freq_hz  = 0.0f;
    }
    else { /* CONST */
        /* convenzione micro: min=max=const */
        c.freq_hz   = 0.0f;
        c.step_amp  = 0.0f;
        c.step_ms   = 0u;
    }

    /* Operazione di STAGING:
    I nuovi parametri di configurazione per il generatore di target sono pronti e sono stati aggiornati nel loop a 1kHz */
    g->cfg_next = c;    // salvo i nuovi parametri di configurazione per il generatore di target in cfg_next
    g->has_next = true; // il flag dice che sono disponibili nuovi parametri di configurazione per il generatore di target
}

/* Operazione di COMMIT
 * Applica davvero i nuovi parametri di configurazione disponibili al generatore di target */
void WaveGen_CommitIfStaged(WaveGen* g)
{
    if (!g || !g->has_next) return;

    g->cfg_active = g->cfg_next; // copia i nuovi parametri di configurazione disponibili nei parametri di configurazione realmente utilizzati dal generatore di target
    g->has_next = false;

    /* resetta alcune variabili runtime per non creare glitch */
    g->phase    = 0.0f;
    g->dir      = +1;
    g->step_cnt = 0;

    /* per step triangle: riparti da min */
    if (g->cfg_active.shape == WGEN_STEPTRIANGLE)
        g->y = g->cfg_active.min_v;

    /* per sin: puoi scegliere se ripartire al centro o mantenere y */
    if (g->cfg_active.shape == WGEN_SIN) {
        float mid = 0.5f * (g->cfg_active.min_v + g->cfg_active.max_v);
        g->y = mid;
    }

    recompute_cached(g);
}

float WaveGen_Step(WaveGen* g)
{
    if (!g) return 0.0f;

    /* Applica l'aggiornamento dei parametri di configurazione per il generatore */
    WaveGen_CommitIfStaged(g);

    const float mn = g->cfg_active.min_v;
    const float mx = g->cfg_active.max_v;

    switch (g->cfg_active.shape)
    {
    case WGEN_SIN: {
        const float mid = 0.5f * (mn + mx);
        const float amp = 0.5f * (mx - mn);
        const float gain = mid + amp;

        g->phase += g->omega * g->dt;
        /* wrap per evitare overflow numerico */
        if (g->phase > 2.0f * (float)M_PI)
            g->phase -= 2.0f * (float)M_PI;

        float s = sinf(g->phase);

        g->y = gain * s;
        g->y = clampf(g->y, mn, mx);
    } break;

    case WGEN_STEPTRIANGLE: {
        /* aggiorna ogni step_ticks */
        g->step_cnt++;
        if (g->step_cnt >= g->step_ticks) {
            g->step_cnt = 0;

            g->y += (float)g->dir * g->cfg_active.step_amp;

            /* rimbalzo su limiti */
            if (g->y >= mx) { g->y = mx; g->dir = -1; }
            if (g->y <= mn) { g->y = mn; g->dir = +1; }
        }
        /* clamp safety */
        g->y = clampf(g->y, mn, mx);
    } break;

    case WGEN_CONST:
    default:
        g->y = mn;  /* convenzione const */
        break;
    }

    return g->y;
}

/* ============================ SignalGens ================================= */

void SignalGens_Init(SignalGens* sg, float sample_rate_hz)
{
    memset(sg, 0, sizeof(*sg));
    WaveGen_Init(&sg->duty,     WGEN_T_DUTY,     sample_rate_hz);
    WaveGen_Init(&sg->current,  WGEN_T_CURRENT,  sample_rate_hz);
    WaveGen_Init(&sg->position, WGEN_T_POSITION, sample_rate_hz);
    sg->last_store_version = 0u;
}

void SignalGens_UpdateFromStore(SignalGens* sg, const TuningStore* s)
{
    if (!sg || !s) return;

    const uint32_t ver = s->version;

    if (ver == sg->last_store_version)
        return; // nessun nuovo tuning da Qt

    sg->last_store_version = ver;

    /* stage (poi commit avverrà nel 10kHz o subito qui se vuoi) */
    if (s->duty.valid)     WaveGen_StageFromTune(&sg->duty,     &s->duty);
    if (s->current.valid)  WaveGen_StageFromTune(&sg->current,  &s->current);
    if (s->position.valid) WaveGen_StageFromTune(&sg->position, &s->position);

    /* Se preferisci commit immediato nel 1kHz, puoi anche fare:
       WaveGen_CommitIfStaged(&sg->duty); ... ecc
       Ma la variante “robusta” è: stage nel 1kHz, commit nel 10kHz in punto noto.
    */
}
