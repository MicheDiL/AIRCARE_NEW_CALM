/*
 * app_control.c
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 */

#ifndef SOURCE_APP_APP_CONTROL_C_
#define SOURCE_APP_APP_CONTROL_C_

#include "plant/plant_control.h"
#include <math.h>

/**************************************** Helpers *****************************************/
static inline float fabsf_local(float x){
    return (x<0.0f)?-x:x;
}

static inline ActDir invert_dir(ActDir d){
    return (d==ACT_DIR_FWD)?ACT_DIR_REV:ACT_DIR_FWD;
}

static inline float clamp(float x, float lo, float hi)
{ return (x<lo)?lo:((x>hi)?hi:x); }

/* È un translator semplice: “voglio I -> quale duty devo mettere?” usando solo il modello ohmico V = I * R_coi
 * Ampere -> dir/duty -> driver (feed-forward open-loop)
 */
static inline void apply_current(PlantControlCtx* c, float u_A)
{
    float I_mag = fabsf_local(u_A);

    // soglia minima: correnti molto piccole -> trattale come zero
    if (I_mag < c->cfg.Imin_A) {
        DrvPwm_Coast();
        return;
    }

    // feed-forward: V = I * R_coil
    float V_ff = I_mag * c->cfg.R_coil_Ohm;

    // converti in duty rispetto alla VMOT reale
    c->duty = V_ff  /c->cfg.Vmot_V;
    if ( c->duty > 1.0f)  c->duty = 1.0f;

    ActDir dir = (u_A >= 0.0f) ? c->cfg.dir_to_increase : invert_dir(c->cfg.dir_to_increase);
    DrvPwm_Drive(dir,  c->duty);        // selezionerà drive-coast o drive-brake a seconda di DrvPwm_SetMode()
}

//Step interno: calcola errore, PD, I_ref_A e aggiorna in_position. Non attua PWM.
static float _step_compute_only(PlantControlCtx* c, uint16_t target_adc, uint16_t y_adc)
{
    // === Errore in Volt: l’EL agisce “a monte” del modello in Volt e non dipende dai gain ===
    float e_lsb = (float)((int32_t)target_adc - (int32_t)y_adc);    // Errore grezzo in LSB (signed)

    if (e_lsb >  c->cfg.e_limit_lsb) e_lsb =  c->cfg.e_limit_lsb;   // Error Limiter (EL) simmetrico in LSB
    if (e_lsb < -c->cfg.e_limit_lsb) e_lsb = -c->cfg.e_limit_lsb;   // Error Limiter (EL) simmetrico in LSB

    float e_V   = e_lsb  * (ADC_VREF_V / ADC_FS_LSB);               // Conversione in Volt dell’errore clampato

    /* ===== PD di posizione -> corrente richiesta [A] =====
           ATTENZIONE: qui CtrlPid_Step è configurato per sputare "Ampere",
           non Volt: Kp/Kd sono in A/V e A/(V/s), Umax = Imax in [A]   */
    float I_ref = CtrlPid_Step(&c->pid, e_V, y_adc);    // ottiene I_ref_A = corrente richiesta (PD posizione in Ampere)

    /* Clamp corrente a Imax_A */
    I_ref = clamp(I_ref, -c->cfg.Imax_A, c->cfg.Imax_A);

    c->I_ref_A = I_ref;   /* pubblica riferimento corrente per il loop 10kHz */

    // In-position detection: banda su posizione + velocità con dwell
    int32_t e_abs = (int32_t)((e_lsb >= 0.0f) ? e_lsb : -e_lsb);

    // ===== Velocità in LSB/ms =====
    float dy = (float)((int32_t)y_adc - (int32_t)c->y_prev_for_vel);
    c->y_prev_for_vel = y_adc;
    float   vabs  = (dy >= 0.0f) ? dy : -dy;

    bool inside_pos = c->in_position ?
        (e_abs <= (int32_t)c->cfg.e_band_exit_lsb) :
        (e_abs <= (int32_t)c->cfg.e_band_enter_lsb);

    bool inside_vel = (vabs <= c->cfg.vel_band_lsb_per_ms);

    if (inside_pos && inside_vel) {
        if (c->dwell_cnt_ms < c->cfg.dwell_ms) c->dwell_cnt_ms++;
    } else {
        c->dwell_cnt_ms = 0u;
    }
    c->in_position = (c->dwell_cnt_ms >= c->cfg.dwell_ms);

    return I_ref;
}
/*********************************************************************************************/

void PlantControl_Init(PlantControlCtx* c, const CtrlPidCfg* pidCfg, const PlantControlCfg* plantCfg)
{
    CtrlPid_Init(&c->pid, pidCfg);
    c->cfg                      = *plantCfg;

    // >>> Propaga la scelta all’ePWM driver <<<
    DrvPwm_SetMode(c->cfg.decay_mode);

    c->started                  = true;
    c->in_position              = false;
    c->dwell_cnt_ms             = 0u;
    c->y_prev_for_vel           = 0u;
    c->target                   = 0u;

    c->duty                     = 0.0f;
    c->duty_freq                = 1.0f;    // 10Hz
    c->duty_gain                = 0.08;     // 8%
    c->duty_sign                = 1;
    c->t_tick_in_step           = 0;
    c->hold_ms_per_step         = 10000;
    c->direzione                = 1;

    // NEW: --- per sinusoide a 10 Hz con tick @10kHz
    c->sin_phase                = 0.0f;

    c->v_tripm1_V               = 0.0f;
    c->correzione_duty          = 1.0f;

    c->I_ref_A                  = 0.0f;
    c->I_meas_A                 = 0.0f;

    // falg che abilita il futuro controllo a cascata finale
    c->cascade_mode             = false;
}

void PlantControl_EnableCascade(PlantControlCtx* c, bool en){
    c->cascade_mode = en;
}

void PlantControl_Reset(PlantControlCtx* c)
{
    CtrlPid_Reset(&c->pid);
    c->in_position              = false;
    c->dwell_cnt_ms             = 0u;
    c->y_prev_for_vel           = 0u;
    c->target                   = 0u;

    c->I_ref_A                  = 0.0f;
    c->I_meas_A                 = 0.0f;

}

// Step @1kHz: genera I_ref_A; se non-cascata, attua PWM via apply_current
void PlantControl_Step1kHz(PlantControlCtx* c, uint16_t target_adc, uint16_t y_adc)
{
    if (!c->started) {
        c->I_ref_A = 0.0f;
        DrvPwm_Coast();
        return;
    }

    float u_cmd_A = _step_compute_only(c, target_adc, y_adc);  // calcola I_ref_A e in_position

    if (c->cascade_mode) {
        /* Modalità CASCATA TRUE: design “galvo like” -> due anelli in cascata.
         * Quindi l’anello esterno si limita a fare da generatore di setpoint di corrente a 1 kHz:
         * posizione -> PD -> I_ref_A (come vuole JSL)
         */
        // L’anello di corrente leggerà c->I_ref_A a 10 kHz.
        if (c->in_position) {
            // opzionale: azzera il riferimento quando sei in posizione
             c->I_ref_A = 0.0f;
        }
        return;
    }

    // Modalità CASCATA FALSE: l’anello esterno controlla direttamente il ponte in feed-forward
    if (c->in_position) {
        // In posizione: optional, riduci corrente a zero o minimo
        apply_current(c, 0.0f);  // Coast o corrente minima
    } else {
        // apply_current prende “I che vorrei” e stima quale tensione media serve, poi la traduce in duty/dir
        apply_current(c, u_cmd_A);
    }
}

float PlantControl_Step1kHz_RefOnly(PlantControlCtx* c, uint16_t target_adc, uint16_t y_adc)
{
    return _step_compute_only(c, target_adc, y_adc);
}

bool  PlantControl_IsInPosition(const PlantControlCtx* c){
    return c->in_position;
}

float PlantControl_LastCommand(const PlantControlCtx* c){
    return c->I_ref_A;
}


// --- ADAPTER che fa il cast del puntatore void* user al tipo reale che vogliamo (es. AppControlCtx*) ---
void PlantControl_ApplyAmpAdapter(void* user, float v)
{
    PlantControlCtx* c = (PlantControlCtx*)user; // cast dal void*
    apply_current(c, v);
}

// Helper per controllare la corrente a LOOP APERTO: genera comandi di duty per il PWM
void PlantSetDutyDir(PlantControlCtx* c){

    if (++c->t_tick_in_step >= c->hold_ms_per_step) {
        c->t_tick_in_step = 0;

        /* prossimo gradino */
        c->duty += c->duty_sign * 0.9;

        /* gestisci inversione ai limiti per ottenere una triangolare a gradini */
        if (c->duty >= 0.9){
            c->duty = 0.9;
            c->duty_sign = -1;
        }
        if (c->duty <= 0){
            c->direzione = !c->direzione;
            c->duty = 0;
            c->duty_sign = +1;
        }

    }

}
void PlantSetDutyDir_Sine10Hz(PlantControlCtx* c)
{
    c->sin_phase += 2.0f*M_PI*c->duty_freq/10000.0f; //c->sin_dphi;

    if (c->sin_phase > 2.0f * M_PI) {
        c->sin_phase -= 2.0f * M_PI;
    }

    float s = sinf(c->sin_phase); // valore della funzione seno calcolata in sin_phase => -1..1

    // Direzione in base al segno
    if (s >= 0.0f) {
        c->direzione = 0;  // es. FWD
    } else {
        c->direzione = 1;  // es. REV
    }

    // Duty proporzionale al valore assoluto della funzione seno
    float mag = fabsf(s);      // 0..1
    c->duty = c->duty_gain * mag;
}

bool LimitCurrent_TRIPM1(PlantControlCtx* c){
    if(c->v_tripm1_V < 9.0f){
        return true;
    }else{
        return false;
    }
}
#endif /* SOURCE_APP_APP_CONTROL_C_ */
