/*
 * pushbutton.h
 *
 *  Created on: 14 nov 2025
 *      Author: mdilucchio
 *
 *      Modulo “gestore pulsante”: fa debounce, conta i multi-click, rileva il long-press e,
 *      in caso di long-press, attiva l’uscita di Harakiri (spegnimento HW)
 */

#ifndef INCLUDE_BUTTON_PUSHBUTTON_H_
#define INCLUDE_BUTTON_PUSHBUTTON_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "HL_gio.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
#define BTN_PORT                    gioPORTB
#define BTN_PIN                     5u
#define BTN_ACTIVE_LOW              false        // pulsante considerato premuto quando il segnale è low (0), tipicamente per via di una pull-up

#define HARAKIRI_PORT               gioPORTB
#define HARAKIRI_PIN                4u

#define BTN_DEBOUNCE_MS             20u      // tempo minimo (ms) in cui una variazione deve rimanere stabile per essere accettata come “vero cambio di stato”
#define BTN_MULTI_CLICK_WIN_MS      400u     // finestra massima tra un rilascio e il successivo click per considerare i click come una stessa “raffica”
#define BTN_LONGPRESS_MS            2000u    // soglia long-press per Harakiri
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef struct {
    uint16_t debounce_ms;        /* default 20 */
    uint16_t longpress_ms;       /* default 2000 */
    uint16_t multi_click_win_ms; /* default 400  */
    bool     btn_active_low;     /* default true */
} BtnCfg;

/* ===== Contesto ===== */
typedef struct {
    BtnCfg cfg;

    /* stato debounced */
    bool     pressed;           // stato del pulsante
    uint16_t db_cnt;            // contatore di debounce: conta quanti ms consecutivi lo stato raw è diverso dallo stato debounced corrente

    /* misura durata pressione e finestra click */
    uint16_t press_ms;          // quanto tempo (ms) il bottone è rimasto in pressed = true nella pressione corrente
    uint16_t window_ms;         // tempo trascorso dall’ultimo rilascio, per capire quando chiudere la “raffica” di click

    /* eventi */
    uint8_t  pending_clicks;     // click contati nella raffica corrente (non ancora pubblicati)
    uint8_t  latched_clicks;     // click “pronti” da leggere con PopClickBurst quando la finestra scade
    bool     long_latched;       // long-press rilevato (latched)

    /* harakiri latch */
    bool     harakiri_fired;     // flag che indica che Harakiri è già stato attivato
} BtnCtx;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
/* ===== API ===== */
void Btn_Init(BtnCtx* b, const BtnCfg* cfg );               // inizializza il contesto, imposta i default o copia dal cfg passato
void Btn_Tick1kHz(BtnCtx* b);                               // funzione da chiamare ogni 1 ms per aggiornare lo stato del bottone

/* Eventi */
bool Btn_PopClickBurst(BtnCtx* b, uint8_t* out_n);          // se c’è una raffica pronta la restituisce
bool Btn_LongPressLatched(const BtnCtx* b);                 // long-press visto almeno una volta
void Btn_ClearLongPress(BtnCtx* b);                         // azzera il flag di long-press latched

/* Azioni */
void Btn_TriggerHarakiri(BtnCtx* b);                        // Azione hardware di spegnimento (porta a 1 GIOB_4)

/* Utility */
static inline bool Btn_HasFired(const BtnCtx* b){
    return b->harakiri_fired;
}   // permette di sapere se è già stata attivata l’uscita di spegnimento
/********************************************************************************************************************/

#endif /* INCLUDE_BUTTON_PUSHBUTTON_H_ */
