/*
 * sp_CurGen.h
 *
 *  Created on: 24 nov 2025
 *      Author: mdilucchio
 */

#ifndef INCLUDE_SETPOINT_SP_CURGEN_H_
#define INCLUDE_SETPOINT_SP_CURGEN_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
/* Triangolare simmetrica tra Imin e Imax, a gradini di delta_I ogni HOLD samples */
typedef struct {
    bool     active;             /* true = test corrente attivo */
    float    i_min_A;            /* ampiezza min (negativa o positiva) */
    float    i_max_A;            /* ampiezza max */
    float    step_A;             /* salto per “gradino” */
    uint16_t hold_ms_per_step;   /* durata di ogni gradino */
    uint16_t t_ticks_in_step;    /* contatore ms step corrente */
    int      dir;                /* +1 sali, -1 scendi */
    float    i_now_A;            /* valore istantaneo del riferimento */
} IRef;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void IRef_Init(IRef* r);

/* start/stop opzionali */
static inline void IRef_Start(IRef* r){
    r->active = true;
}
static inline void IRef_Stop (IRef* r){
    r->active = false;
}

/* Tick a 1 kHz: avanza la triangolare a gradini */
void IRef_Tick1kHz(IRef* r);

/* Getter */
static inline float IRef_GetA(const IRef* r){
    return r->i_now_A;
}
/********************************************************************************************************************/

#endif /* INCLUDE_SETPOINT_SP_CURGEN_H_ */
