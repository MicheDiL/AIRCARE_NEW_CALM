/*
 * ctrl_pid.h
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 *
 *      Controllore di posizione assi X ed Y:
 *      Anello di controllo a loop chiuso per  del tipo 0, solo con elementi Proporzionale e Derivativo (Controllo PD), senza
        elemento integratore. La variabile di controllo dello stadio di uscita è proporzionale alla differenza tra posizione corrente e
        posizione target (Errore) moltiplicata per il valore del parametro Kp, a cui va sottratta algebricamente la
        velocità corrente (derivata dalla storia della posizione corrente) moltiplicata per il valore del parametro Kd.
 */

#ifndef INCLUDE_CONTROLLER_CTRL_PID_H_
#define INCLUDE_CONTROLLER_CTRL_PID_H_

/*************************************************** Include Files **************************************************/
#include "sampling_adc/drv_adc.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
// PARAMETRI DEL CONTROLLORE PD PER CONTROLLO POSIZIONE XY DI JSL
typedef struct {
    float Kp_A;          // [A/V]   guadagno proporzionale
    float Kd_A;          // [A/(V/s)] guadagno derivativo
    float Ts_s;                 // tempo di campionamento in secondi -> 0.001s
    float TfD_s;                // costante del filtro derivativo (1° ordine) -> 0.015s
    bool  d_on_meas;            // se true, il termine D lavora su -dy/dt (evita il “derivative kick” sui cambi di set-point)
} CtrlPidCfg;

// STATO RUNTIME DEL PD
typedef struct {
    float d_filt;               // stato del filtro D
    float e_prev_V;             // errore precedente
    uint32_t y_prev_adc;        // misura ADC precedente
} CtrlPidState;

// CONFIGURAZIONE DEL PD + STATO DEL PD
typedef struct {
    CtrlPidCfg   cfg;
    CtrlPidState st;
} CtrlPidCtx;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void  CtrlPid_Init (CtrlPidCtx* c, const CtrlPidCfg* cfg);
void  CtrlPid_Reset(CtrlPidCtx* c);

/**
 * @param e_V       errore in Volt
 * @param y_adc_cur misura ADC corrente (per D-on-measurement)
 * @return          corrente richiesta [A] (P + D), nessuna saturazione interna
 */
float CtrlPid_Step(CtrlPidCtx* c, float e_V, uint16_t y_adc_cur);   // un regolatore PD di posizione che sputa una corrente richiesta

/* Accessor ai default (static const definiti nel .c dedicato) */
const CtrlPidCfg* CtrlPid_DefaultCfg(void);
/********************************************************************************************************************/

#endif /* INCLUDE_CONTROLLER_CTRL_PID_H_ */
