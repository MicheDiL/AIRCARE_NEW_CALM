/*
 * sp_gen.c
 *
 *  Created on: 3 nov 2025
 *      Author: mdilucchio
 */

#include "setpoint/sp_gen.h"
#include "sampling_adc/drv_adc.h"
/* ===== Helpers ===== */
static inline uint16_t clamp12(uint32_t x){
    return (x > POSX_SPAN_LSB) ? POSX_SPAN_LSB : (uint16_t)x;
}
static inline uint16_t nz_u16(uint16_t x, uint16_t fallback){
    return (x == 0u) ? fallback : x;            // assicura che step_lsb e advance_ms siano >=1
}

/* ===== Default in ROM ===== */
static const SpGenCfg kSpg_Default = {
    .wave           = SP_WAVE_TRIANGLE,
    .min_lsb        = 0,
    .max_lsb        = POSX_SPAN_LSB,
    .step_lsb       = 32,
    .advance_ms     = 5000,
};

const SpGenCfg* SpGen_DefaultCfg(void){
    return &kSpg_Default;                       // restituisce un puntatore const alla cfg di default
}

/* ===== API ===== */
void SpGen_Init(SpGen* g, const SpGenCfg* cfg)
{
    // Inizializza la copia locale di configurazione: se cfg è NULL, usa i default
    g->cfg.min_lsb    = cfg         ? cfg->min_lsb       : kSpg_Default.min_lsb;
    g->cfg.max_lsb    = cfg         ? cfg->max_lsb       : kSpg_Default.max_lsb;
    // Passa step_lsb/advance_ms per nz_u16(..., 1) -> mai 0
    g->cfg.step_lsb   = nz_u16(cfg  ? cfg->step_lsb      : kSpg_Default.step_lsb,   1u);
    g->cfg.advance_ms = nz_u16(cfg  ? cfg->advance_ms    : kSpg_Default.advance_ms, 1u);

    // se i limiti sono invertiti li scambia
    if (g->cfg.min_lsb > g->cfg.max_lsb){
        uint16_t t = g->cfg.min_lsb;
        g->cfg.min_lsb = g->cfg.max_lsb;
        g->cfg.max_lsb = t;
    }

    // imposta lo stato iniziale
    g->current = POSX_SPAN_LSB/2;   // ~1100
    g->acc_ms  = 0;
    g->enabled = false;
    g->tri_up  = true;
}

void SpGen_Arm(SpGen* g, uint16_t start_lsb)
{
    // primo valore richiesto: 2048 dopo calibrazione
    g->current = clamp12(start_lsb);

    // Decide la direzione iniziale
    if (g->current <= g->cfg.min_lsb) g->tri_up = true;         // Se parti in fondo al min, sali
    else if (g->current >= g->cfg.max_lsb) g->tri_up = false;   // Se parti in max, scendi
    else g->tri_up = true;                                      // Altrimenti, scegli salita come default

    g->acc_ms  = 0;
    g->enabled = true;;
}

void SpGen_Disarm(SpGen* g)
{
    g->enabled = false;                                         // Disabilita l’avanzamento (il valore current resta “congelato”)
}

void SpGen_SetStepLsb(SpGen* g, uint16_t step_lsb)
{
    g->cfg.step_lsb = nz_u16(step_lsb, 1u);                     // Setter runtime dell’ampiezza del gradino; garantisce >= 1 lsb
}

void SpGen_SetAdvanceMs(SpGen* g, uint16_t advance_ms)
{
    g->cfg.advance_ms = nz_u16(advance_ms, 1u);                 // Setter runtime del ritmo degli step; garantisce >= 1 ms
}

static void _advance(SpGen* g)
{
    // Funzione interna (file-scope) che esegue un gradino della triangolare, leggendo i parametri in variabili locali const (valori più veloci da usare)
    const uint16_t lo = g->cfg.min_lsb;
    const uint16_t hi = g->cfg.max_lsb;
    const uint16_t st = g->cfg.step_lsb;

    if (g->tri_up) {
        if ((uint32_t)g->current + st >= hi) {                  // il cast a uint32_t nell’addizione: evita overflow prima del confronto
            g->current = hi;
            g->tri_up = false;                                  // inverti direzione
        } else {
            g->current = (uint16_t)(g->current + st);
        }
    } else {
        if (g->current <= lo + st) {
            g->current = lo;
            g->tri_up = true;                                   // inverti direzione
        } else {
            g->current = (uint16_t)(g->current - st);
        }
    }
}

void SpGen_Tick1ms(SpGen* g)
{
    if (!g->enabled) return;

    /* Scheduler di avanzamento a tempo */
    if (++g->acc_ms >= g->cfg.advance_ms) {                     // incrementa l’accumulatore ms
        g->acc_ms = 0;                                          // resetta l'accumulatore
        _advance(g);                                            // fa un gradino
    }
}

uint16_t SpGen_Get(const SpGen* g)
{
    return g->current;
}


/* ===== Opzione B: usare l'hook dell'RTI ===== */
#ifdef USE_RTI_HOOK
#include "control_loop/drv_rti.h"
static SpGen* s_bound = 0;
static void _spg_hook(void){
    if (s_bound) SpGen_Tick1ms(s_bound);
}
void SpGen_RegisterAsRtiHook(SpGen* g)
{
    s_bound = g;
    DrvRti_RegisterHook(_spg_hook);
}
#else
void SpGen_RegisterAsRtiHook(SpGen* g){ (void)g; /* noop se non usi hook */ }
#endif
