/*
 * current_ctrl.c
 *
 *  Created on: 21 nov 2025
 *      Author: mdilucchio
 */

#include "controller/current_ctrl.h"

static inline float clampf(float x, float lo, float hi){
    return (x<lo)?lo:((x>hi)?hi:x);
}

/* Dimensionamento PI su plant R–L:
 *    Kp = L * omega_c
 *    Ki = R * omega_c
 * con omega_c = 2pi*fc.
 * Kaw ~ omega_c (o 2*omega_c) per tracking anti-windup rapido ma stabile.
 */
static const CurPiCfg kCur_Default = {
  .Ts_s           = 0.0001f,            // 0.0001f @10kHz (o 0.0002f @5kHz)
  .Kp             = 10, //L_H   * 2.0f * M_PI * FC_HZ/100,   // V/A
  .Ki             = 0.01, //R_OHM * 2.0f * M_PI * FC_HZ/100,   // V/(A·s)
  .Kaw            = 0, //2.0f * M_PI * FC_HZ,   // 1/s
  .Vmax_V         = 12.0f,
  .Imin_A         = 0.03f
};

const CurPiCfg* CurPi_DefaultCfg(void)
{
    return &kCur_Default; // restituisce un puntatore const a un oggetto static const (immutabile per i chiamanti) definito sopra
}


void CurPi_Init(CurPiCtx* c, const CurPiCfg* cfg){
    c->cfg = *cfg;
    CurPi_Reset(c);
}
void CurPi_Reset(CurPiCtx* c){
    c->I_V = 0.0f;
    c->e_A = 0.0f;
    c->u_unsat_V = 0.0f;
    c->u_sat_V   = 0.0f;
}

float  CurPi_Step(CurPiCtx* c, float i_ref_A, float i_meas_A)
{
    // Richiesta molto piccola -> zero corrente
    if (fabsf(i_ref_A) < c->cfg.Imin_A) {
        c->I_V       = 0.0f;
        c->e_A       = 0.0f;
        c->u_unsat_V = 0.0f;
        c->u_sat_V   = 0.0f;
        return 0.0f;
    }

    float e = i_ref_A - i_meas_A;
    c->e_A = e;

    float u_unsat = c->cfg.Kp * e + c->I_V;                   // [V]
    float u_sat   = clampf(u_unsat, -c->cfg.Vmax_V, c->cfg.Vmax_V);

    float Ts = c->cfg.Ts_s;
    c->I_V += (c->cfg.Ki * Ts) * e + (c->cfg.Kaw * Ts) * (u_sat - u_unsat);

    c->u_unsat_V = u_unsat;
    c->u_sat_V   = u_sat;

    return u_sat;   // comando tensione “ideale” per la bobina

}
