/*
 * app_calibrator.h
 *
 *  Created on: 3 nov 2025
 *      Author: mdilucchio
 *
 *      Esegue uno sweep positivo/negativo a step di tensione, attende, controlla lo spostamento ADC e misura Vdead_pos / Vdead_neg
        ATTENZIONE: Questo lo facevo per un motore DC Maxon. Non serve per attuatore galvanometrico CALM poichè dead-zone quasi nulla!
 */

#ifndef INCLUDE_APP_APP_CALIBRATOR_H_
#define INCLUDE_APP_APP_CALIBRATOR_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>     // fornisce tipi a dimensione fissa (uint16_t, uint32_t…)
#include <stdbool.h>    // abilita il tipo bool con i literal true/false
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef struct {
    float    vstep_V;          // es. 0.09
    uint16_t hold_ms;          // 200
    uint16_t settle_ms;        // 2000
    uint16_t delta_lsb_thr;    // 35
    float    vmot_V;           // (per clamp duty = |u|/Vmot)
} AppCalibCfg;

typedef enum {
    CAL_INIT=0,
    CAL_POS_APPLY,
    CAL_POS_HOLD,
    CAL_POS_CHECK,
    CAL_ZERO_SETTLE,
    CAL_NEG_APPLY,
    CAL_NEG_HOLD,
    CAL_NEG_CHECK,
    CAL_DONE
} AppCalibState;

typedef struct {
    AppCalibState st;
    uint16_t hold_cnt, settle_cnt;
    uint16_t y0_pos, y0_neg;
    uint16_t delta_pos_adc, delta_neg_adc;
    float    v_cmd;            // Volt attuali
    float    vdead_pos_V, vdead_neg_V;
    bool     done;
} AppCalibCtx;

typedef void (*AppApplyVoltFn)(void* user, float v);    // Callback con user data (context)
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
/* default base in ROM + accessor */
const AppCalibCfg* AppCalib_DefaultCfg(void);

/* API */
void AppCalib_Init (AppCalibCtx* c);
void AppCalib_Reset(AppCalibCtx* c);

/**
 * Step @1 kHz.
 * applyVolt: funzione che accetta Volt e li manda all’attuatore (Volt->(dir,duty)->PWM).
 */
void AppCalib_Step1kHz(AppCalibCtx* c, const AppCalibCfg* cfg, uint16_t y_adc, AppApplyVoltFn applyVolt, void* user);

/* Getter risultati */
static inline bool  AppCalib_IsDone(const AppCalibCtx* c){ return c->done; }
static inline float AppCalib_VdeadPos(const AppCalibCtx* c){ return c->vdead_pos_V; }
static inline float AppCalib_VdeadNeg(const AppCalibCtx* c){ return c->vdead_neg_V; }
/********************************************************************************************************************/



#endif /* INCLUDE_APP_APP_CALIBRATOR_H_ */
