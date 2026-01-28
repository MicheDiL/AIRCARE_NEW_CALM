/*
 * sp_CurGen.c
 *
 *  Created on: 24 nov 2025
 *      Author: mdilucchio
 */

#include "setpoint/sp_CurGen.h"

static inline float _clampf(float x,float lo,float hi){ return (x<lo)?lo:((x>hi)?hi:x); }

void IRef_Init(IRef* r){
    r->active           = true;
    r->i_min_A          = -0.40f;   // 400mA
    r->i_max_A          = +0.40f;   // -400mA
    r->step_A           = 0.04f;    // 40mA
    r->hold_ms_per_step = 1000;     // 100ms per step
    r->t_ticks_in_step  = 0;
    r->dir              = +1;
    r->i_now_A          = 0.0f;
}

void IRef_Tick1kHz(IRef* r)
{
    if (!r->active) return;

    if (++r->t_ticks_in_step >= r->hold_ms_per_step) {
        r->t_ticks_in_step = 0;

        /* prossimo gradino */
        r->i_now_A += r->dir * r->step_A;

        /* gestisci inversione ai limiti per ottenere una triangolare a gradini */
        if (r->i_now_A >= r->i_max_A)   {
            r->i_now_A = r->i_max_A;
            r->dir = -1;
        }
        if (r->i_now_A <= r->i_min_A)   {
            r->i_now_A = r->i_min_A;
            r->dir = +1;
        }
    }

}
