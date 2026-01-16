/*
 * current_ctrl.h
 *
 *  Created on: 21 nov 2025
 *      Author: mdilucchio
 *
 *      PI di corrente (anello interno, 10 kHz).
 *      Il PI produce direttamente un comando in Volt verso il ponte H, poi mappi Volt->duty
 */

#ifndef INCLUDE_CONTROLLER_CURRENT_CTRL_H_
#define INCLUDE_CONTROLLER_CURRENT_CTRL_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <math.h>
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
#define L_H       2.82e-4f      // [Henry]
#define R_OHM     7.29f         // [Ohm]
#define FC_HZ     800           // [Hz]
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef struct {
    float Ts_s;       // periodo di campionamento [s] (es. 0.0001 a 10 kHz)
    float Kp;         // [V/A] guadagno proporzionale
    float Ki;         // [V/(A·s)] guadagno integrale
    float Kaw;        // coefficiente anti-windup (back-calculation)
    float Vmax_V;     // tensione massima modulabile (es. VMOT)
    float Imin_A;     // soglia sotto la quale I_ref è considerata 0
} CurPiCfg;

typedef struct {
    CurPiCfg cfg;
    float I_V;        // stato integratore [V]
    float e_A;        // ultimo errore [A]
    float u_unsat_V;  // uscita PI non saturata [V]
    float u_sat_V;    // uscita PI saturata [V]
} CurPiCtx;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void  CurPi_Init (CurPiCtx* c, const CurPiCfg* cfg);
void  CurPi_Reset(CurPiCtx* c);

/**
 * @return comando in Volt da applicare alla bobina (sat a ±Vmax_V)
 */
float CurPi_Step(CurPiCtx* c, float i_ref_A, float i_meas_A);       // restituisce un V_cmd già saturato a ±VMOT, con integratore gestito

const CurPiCfg* CurPi_DefaultCfg(void);
/********************************************************************************************************************/

#endif /* INCLUDE_CONTROLLER_CURRENT_CTRL_H_ */
