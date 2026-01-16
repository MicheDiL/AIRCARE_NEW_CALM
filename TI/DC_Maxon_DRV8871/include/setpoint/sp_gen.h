/*
 * sp_gen.h
 *
 *  Created on: 3 nov 2025
 *      Author: mdilucchio
 *
 *      Genera un target ADC 12-bit (0..4095) in modo ciclico (saw/tri/square o tabellare).
 *      Rimane disarmato finché l’App non decide di avviarlo (dopo calibrazione).
 *      Il primo valore quando lo armi è 2048 (metà scala).
 *      Aggiorna il target ogni N millisecondi (configurabile) senza bloccare il sistema.
 */

#ifndef INCLUDE_SETPOINT_SP_GEN_H_
#define INCLUDE_SETPOINT_SP_GEN_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
// Forme supportate (cicliche in 0..4095) //
typedef enum {
    SP_WAVE_TRIANGLE = 0,       // triangolo 0->4095->0...
    //SP_WAVE_SAW,              // rampa 0->4095->0
    //SP_WAVE_HOLD,             // costante sul valore corrente
    //SP_WAVE_SQUARE,           // 0 / 4095 con dwell
    //SP_WAVE_TABLE             // sequenza personalizzata (tabella)
} SpWave;

// Configurazione //
typedef struct {
    SpWave   wave;              // forma
    uint16_t min_lsb;
    uint16_t max_lsb;
    uint16_t step_lsb;          // ampiezza del gradino (quanti LSB si avanza ogni “step”) (es. 32)
    uint16_t advance_ms;        // ogni quanti ms avanzare (es. 4 ms)
} SpGenCfg;

// Stato/contesto //
typedef struct {
    SpGenCfg cfg;               // copia della configurazione
    uint16_t   current;         // target attuale (0..4095)
    uint16_t   acc_ms;          // contatore di millisecondi per sapere quando fare un gradino
    bool       enabled;         // true se armato
    bool       tri_up;          // direzione dell'onda (true=salita)
} SpGen;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
// Default in ROM + accessor //
const SpGenCfg* SpGen_DefaultCfg(void);

// API //
void     SpGen_Init   (SpGen* g, const SpGenCfg* cfg);          // inizializza stato e copia la cfg
void     SpGen_Arm    (SpGen* g, uint16_t start_lsb);           // abilita il generatore e imposta il primo valore (es. 2048).
void     SpGen_Disarm (SpGen* g);                               // disabilita l’avanzamento

void     SpGen_SetStepLsb  (SpGen* g, uint16_t step_lsb);       // cambia ampiezza gradino runtime
void     SpGen_SetAdvanceMs(SpGen* g, uint16_t advance_ms);     // cambia velocità gradini runtime

void     SpGen_Tick1ms(SpGen* g);                               // da chiamare ogni 1ms (nel ramo 1kHz o via hook)
uint16_t SpGen_Get    (const SpGen* g);                         // leggi il setpoint attuale

/* Opzione B: registrazione come RTI hook */
void SpGen_RegisterAsRtiHook(SpGen* g); /* usa DrvRti_RegisterHook() internamente */
/********************************************************************************************************************/

#endif /* INCLUDE_SETPOINT_SP_GEN_H_ */
