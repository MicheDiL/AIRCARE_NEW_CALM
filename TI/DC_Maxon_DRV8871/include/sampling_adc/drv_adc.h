/*
 * drv_adc.h
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 *
 *
     * Driver ADC1 (Group1) con trigger hardware EPWM1 SOCA.
     * - ISR EOC @ 50 kHz: accumula e media ogni 'oversample' campioni.
     * - API non bloccante: DrvAdc_TryRead1kHz() fornisce l'ultimo valore "pulito".
     * - Nessuna dipendenza da unità fisiche: valore grezzo 12-bit (0..4095).
     *
 */

#ifndef INCLUDE_SAMPLING_ADC_DRV_ADC_H_
#define INCLUDE_SAMPLING_ADC_DRV_ADC_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>     // fornisce tipi a dimensione fissa (uint16_t, uint32_t…)
#include <stdbool.h>    // abilita il tipo bool con i literal true/false

#include "HL_adc.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
#define ADC_VREF_V        3.3f     // Il riferimento ADC del RM57 è nel dominio 3.3VA
#define ADC_FS_LSB        4095u   // È un ADC a 12 bit: il massimo conteggio è 4095
#define OSR_ENC1          50u       // Oversampling ratio per l'anello di posizione 50k/50 = 1kHz
#define OSR_CURR          5u        // Oversampling ratio per l'anello di corrente 50k/5  = 10kHz

#ifndef POSX_SPAN_LSB
#define POSX_SPAN_LSB     2150u     // misurato: 1.774 V -> ~2150 LSB
#endif

// Una media a finestra mobile (MA) nell’acquisizione del segnale di posizione serve a ridurre suscettibilità ai rumori ed aumentare la risoluzione
#ifndef POS_MA_POW2_MAX
#define POS_MA_POW2_MAX   4u                        // Nmax = 2^4 = 16
#endif
#define POS_MA_NMAX       (1u << POS_MA_POW2_MAX)   // 1u spostato a sinistra di POS_MA_POW2_MAX bit: modo tipico, molto compatto, per parametrizzare grandezze che devono essere potenze di 2 (dimensioni di buffer, finestre di media mobile ecc...)

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
/* === Mappatura logica canali Group1 (come da HAL) ===
 *  0: PCM (INA240), 1: POS (ENC1), 2: ACM (IPROPI->R152)
 */
typedef enum {
    ADC_CH_PCM  = 0,        // INA240 (precise current)
    ADC_CH_ENC1,            // ENC1 (posizione)
    ADC_CH_ACM,             // IPROPI (approx current)
    ADC_CH_TRIP,            // TRIPM1 (MOT1P tap -> AD1IN3)
    ADC_CH_COUNT
} AdcChIdx;

typedef struct {
  // === NEW: Moving Average posizione ===
  uint8_t  pos_ma_pow2;       // NEW: finestra media mobile (ma) su posizione = 2^pos_moving_avg_pow2
  uint16_t pos_buf[POS_MA_NMAX]; /* buffer circolare */
  uint32_t pos_acc;              /* somma corrente */
  uint16_t pos_idx;              /* indice circolare [0..N-1] */
  uint16_t pos_mask;             /* maschera che serve per far ripartire il conteggio degli indici del buffer da 0 quando (idx + 1) > (length del buffer) */
  uint16_t pos_fill;             /* quanti campioni validi (per start-up) */

  uint16_t enc1_lsb;
} AdcPos;

typedef struct {
    uint16_t acm1_lsb;
    uint16_t pcm1_lsb;
    uint16_t trip1_lsb;
} AdcCurrents;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
/** Inizializza ADC1 Group1: abilita notifiche e avvia conversioni. */
void DrvAdc_Init(void);

// Legge (non bloccante) l'ultimo campione ENC1 "pulito" a 1 kHz.
// @return true se un nuovo campione ENC1 è stato pubblicato dall'ISR dopo l'ultima lettura.
bool DrvAdc_ReadEnc_1kHz(AdcPos* out);
bool DrvAdc_ReadCurr_10kHz(AdcCurrents* out);

/* Peek (non consumano) */
uint16_t DrvAdc_PeekLast_Pcm1(void);
uint16_t DrvAdc_PeekLast_Enc1(void);
uint16_t DrvAdc_PeekLast_Acm1(void);

// NEW Moving Average posizione
void DrvAdc_Pos_Init(AdcPos* c);
void DrvAdc_Pos_Reset(AdcPos* c);
void PlantControl_SetPosMaPow2(AdcPos* c, uint8_t p2); // Setter runtime
/********************************************************************************************************************/

#endif /* INCLUDE_SAMPLING_ADC_DRV_ADC_H_ */
