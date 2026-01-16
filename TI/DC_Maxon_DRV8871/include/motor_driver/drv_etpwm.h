/*
 * drv_etpwm.h
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 *
 *      ePWM1
 *      - Time-base: Up-Down, frequenza target 50 kHz (TBCLK=VCLK3/(HSP*CLK)).
 *      - Shadow load su CMPA/CMPB (load @ CTR=Zero).
 *      - Action Qualifier (PWM centrato):
 *          A: CAU=Set, CAD=Clear.
 *          B: CBU=Set, CBD=Clear.
 *      - SOCA abilitata su CTR=PRD, First Event (per l’ADC).
 */

#ifndef INCLUDE_MOTOR_DRIVER_DRV_ETPWM_H_
#define INCLUDE_MOTOR_DRIVER_DRV_ETPWM_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "HL_etpwm.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
#define ETPWM_FREQ_HZ     50000u            // segnali ePWM a 50 kHz
#define VCLK3_HZ          75000000u         // Usiamo VCLK3=75 MHz
#define CLKDIV            ClkDiv_by_1
#define HSPCLKDIV         HspClkDiv_by_1
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef enum {
    ACT_DIR_FWD = 0,
    ACT_DIR_REV = 1
} ActDir;

typedef enum {
    DECAY_COAST = 0,                        // logica inziale:       drive-coast (un ramo LOW fisso, l'altro PWM attivo-alto)
    DECAY_BRAKE = 1                         // logica datasheet TI:  drive-brake (un ramo HIGH fisso, l'altro PWM attivo-basso)
} DecayMode;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void DrvPwm_Init(void);
void DrvPwm_SetMode(DecayMode m);
DecayMode DrvPwm_GetMode(void);

void DrvPwm_Coast(void);
void DrvPwm_Brake(void);
void DrvPwm_Drive(ActDir dir, float duty01);

/* Utility per debug/calibrazione: set duty raw dei due canali [0..1] */
void DrvPwm_SetDutyRawAB(float dutyA01, float dutyB01);
/********************************************************************************************************************/

#endif /* INCLUDE_MOTOR_DRIVER_DRV_ETPWM_H_ */
