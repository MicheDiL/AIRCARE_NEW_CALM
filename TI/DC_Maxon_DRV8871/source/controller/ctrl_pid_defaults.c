/*
 * ctrl_pid_defaults.c
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 */

#include "controller/ctrl_pid.h"

/* Unico punto di verità in ROM */
static const CtrlPidCfg kPid_Default = {    // NEW: CONTROLLO IN CORRENTE: questo PID è ora un generatore di corrente
    .Kp_A           = 15.0f,    // NEW: CONTROLLO IN CORRENTE: Fai uscire il PD in Ampere, non in Volt => Kp_V è da intendersi come A/V
    .Kd_A           = 0.09f,    // NEW: CONTROLLO IN CORRENTE: Fai uscire il PD in Ampere, non in Volt => Kd_V è da intendersi come A/(V/s)
    .Ts_s           = 0.001f,
    .TfD_s          = 0.015f,
    .d_on_meas      = true      // usiamo la velocità dalla storia della posizione
};

const CtrlPidCfg* CtrlPid_DefaultCfg(void)
{
    return &kPid_Default; // restituisce un puntatore const a un oggetto static const (immutabile per i chiamanti) definito sopra
}


