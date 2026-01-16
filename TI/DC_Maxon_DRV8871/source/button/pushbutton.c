/*
 * pushbutton.c
 *
 *  Created on: 14 nov 2025
 *      Author: mdilucchio
 */

#include "button/pushbutton.h"

/* === Helpers === */
static inline uint32_t _gio_get(const gioPORT_t* p, uint32_t bit){
    return gioGetBit((gioPORT_t*)p, bit);
}
static inline void _gio_set(gioPORT_t* p, uint32_t bit, uint32_t val){
    gioSetBit(p, bit, (val & 1u));
}
static inline bool _btn_phys_pressed(bool active_low){
    uint32_t v = _gio_get(BTN_PORT, BTN_PIN);
    return active_low ? (v==0u) : (v!=0u);
}


void Btn_Init(BtnCtx* b, const BtnCfg* cfg /*nullable*/){
    /* Nota: assumiamo pin già configurati da HALCoGen:
       - GIOB_5 input (pull-up se active-low),
       - GIOB_4 output, default LOW (harakiri disattivo).
    */

    _gio_set(HARAKIRI_PORT, HARAKIRI_PIN, 0u);              // Forziamo il livello di Harakiri a 0 per sicurezza

    // Se cfg == NULL, usa i default
    b->cfg.debounce_ms        = cfg ? cfg->debounce_ms        : BTN_DEBOUNCE_MS;
    b->cfg.longpress_ms       = cfg ? cfg->longpress_ms       : BTN_LONGPRESS_MS;
    b->cfg.multi_click_win_ms = cfg ? cfg->multi_click_win_ms : BTN_MULTI_CLICK_WIN_MS;
    b->cfg.btn_active_low     = cfg ? cfg->btn_active_low     : BTN_ACTIVE_LOW;

    b->pressed        = false;
    b->db_cnt         = 0u;
    b->press_ms       = 0u;
    b->window_ms      = 0u;
    b->pending_clicks = 0u;
    b->latched_clicks = 0u;
    b->long_latched   = false;
    b->harakiri_fired = false;
}

void Btn_Tick1kHz(BtnCtx* b){
    bool raw = _btn_phys_pressed(b->cfg.btn_active_low);    // stato grezzo del pulsante. Il raw deve rimanere diverso dallo stato attuale per debounce_ms tick consecutivi per essere considerato un vero cambio

    if (raw != b->pressed) {                                // valuta eventuale cambio di stato del bottone
        if (++b->db_cnt >= b->cfg.debounce_ms) {            // accetta il cambio di stato del pulsante
            b->pressed = raw;                               // aggiorna b->pressed = raw
            b->db_cnt  = 0u;                                // azzera db_cnt

            if (b->pressed) {                               // il nuovo stato del bottone è "bottone premuto"
                /* rising edge (pressed) */
                b->press_ms = 0u;                           // press_ms viene azzerato, iniziamo a conteggiarne la durata
            } else {                                        // il nuovo stato del bottone è "bottone rilasciato"
                /* falling edge (released) */
                if (!b->long_latched) {

                    if (b->pending_clicks < 255u) b->pending_clicks++;  // la pressione precedente non è stata abbastanza da rilevare un long-press quindi la conteggiamo come semplice click
                    b->window_ms = 0u;                      // parte la finestra per poter riconoscere, eventualmente, un secondo/terzo click nella stessa raffica
                }

                b->long_latched = false;                    // Si azzera comunque long_latched per la prossima pressione
            }
        }
    } else {                                                // nessun cambio di stato del pulsante
        b->db_cnt = 0u;                                     // ogni rimbalzo che riporta raw uguale a b->pressed resetta il contatore
    }

    /* Press duration / Long-press detection */
    if (b->pressed) {                                       // Se il bottone è attualmente premuto
        if (b->press_ms < 60000u) b->press_ms++;            // Incrementa press_ms ogni ms (saturando a 60000 per evitare overflow)
        if (!b->long_latched && (b->press_ms >= b->cfg.longpress_ms)) { // lon-press non ancora segnalato e press_ms supera longpress_ms

            b->long_latched = true;                         // Evento logico che puoi usare nel SW

        }
    } else {
        /* gestione finestra multi-click */
        if (b->pending_clicks > 0u) {                       // Se ci sono click pendenti
            if (b->window_ms < 60000u) b->window_ms++;
            if (b->window_ms >= b->cfg.multi_click_win_ms) {
                /* pubblica la raffica */
                b->latched_clicks = b->pending_clicks;      // “freeza” il totale dei click consecutivi della raffica che posso leggere con Btn_PopClickBurst
                b->pending_clicks = 0u;
                b->window_ms      = 0u;
            }
        }
    }
}

bool Btn_PopClickBurst(BtnCtx* b, uint8_t* out_n)
{
    if (b->latched_clicks == 0u) return false;              // nessuna raffica pronta -> ritorna false
    if (out_n) *out_n = b->latched_clicks;                  // scrive il valore in *out_n se non è NULL
    b->latched_clicks = 0u;
    return true;
}

bool Btn_LongPressLatched(const BtnCtx* b)
{
    //return b->harakiri_fired || b->long_latched;
    return b->long_latched;
}

void Btn_ClearLongPress(BtnCtx* b)
{
    b->long_latched = false;
}

void Btn_TriggerHarakiri(BtnCtx* b)
{
    b->long_latched = false;
    _gio_set(HARAKIRI_PORT, HARAKIRI_PIN, 1u);              // 1 = spegni (Harakiri)
    //b->harakiri_fired = true;
}
