/*
 * app_control.h
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 *
 *      Contiene le politiche di attuazione del controllo PD per gli assi X e Y (dead-zone, verso, relax in-position)
 */

#ifndef INCLUDE_APP_PLANT_CONTROL_H_
#define INCLUDE_APP_PLANT_CONTROL_H_

/*************************************************** Include Files **************************************************/
#include "motor_driver/drv_etpwm.h"
#include "controller/ctrl_pid.h"
#include <math.h>
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
// CONFIGURAZIONE DEL PLANT
typedef struct {

    /* Plant */
    float Vmot_V;                   // ALIMENTAZIONE DRV8231a

    /* In-position detection su posizione */
    uint16_t e_band_enter_lsb;      /* 18 */
    uint16_t e_band_exit_lsb;       /* 22 */
    float    vel_band_lsb_per_ms;   /* Banda di velocità: 2.0 */
    uint16_t dwell_ms;              /* Permanenza minima in banda prima di dire “in posizione”: 200 */

    /* NEW: Limitatore di errore (magnitudine) in LSB */
    uint16_t e_limit_lsb;     /* es. 400..800 LSB, da tarare */

    // CONTROLLO IN CORRENTE
    float R_coil_Ohm;   // resistenza bobina JSL, 3.2 Ohm
    float Imax_A;       // corrente max ammessa (es. 1.5 A)
    float Imin_A;       // opzionale: min corrente “utile. Sotto questo valore consideri "zero" (es. 0.03–0.05 A)

    /* Direzione e ponte H  */
    ActDir   dir_to_increase;       /* ACT_DIR_FWD/REV */
    DecayMode decay_mode;           /* DECAY_COAST/BRAKE */

} PlantControlCfg;

typedef struct {

    CtrlPidCtx   pid;           // Incapsula config+stato del PID
    PlantControlCfg cfg;          // Copia della configurazione applicativa

    /* Stato runtime */
    bool     started;           // Consente di “armare” il controllo dopo la calibrazione
    bool     in_position;       // Stato corrente di in-position
    uint16_t dwell_cnt_ms;      // Contatore di permanenza in banda
    uint16_t y_prev_for_vel;    // Per stimare |dy|/ms
    uint16_t target;

    //duty
    float duty;
    float duty_freq;
    float duty_gain;
    int duty_sign;
    uint16_t t_tick_in_step;        // utility contatore
    uint16_t hold_ms_per_step;      // utility contatore
    bool direzione;                 // utility direzione

    // NEW: --- parametri per sinusoide ---
    float    sin_phase;      // fase corrente [rad]

    float    I_ref_A;         /* corrente richiesta dall’anello posizione [A] */
    float    I_meas_A;        /* corrente misurata [A] (da PCM) */

    // Fusibile termico nodo motore (TRIPM1)/
    float v_tripm1_V;                   // tensione ai capi del fusibile termico
    float correzione_duty;             // 0..1 stimata

    /* Selettore della modalità di controllo:
     * FALSE -> anello singolo, corrente “open-loop” (feed-forward)
     * TRUE ->  due anelli in cascata, corrente “closed-loop” (vero controllo di corrente)
    */
    bool cascade_mode;

} PlantControlCtx;  // Ctx sta per context ovvero una struttura che contiene tutto lo stato e la configurazione di un modulo/istanza
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void PlantControl_Init (PlantControlCtx* c, const CtrlPidCfg* pidCfg, const PlantControlCfg* appCfg);
void PlantControl_Reset(PlantControlCtx* c);

/* Step @1kHz: aggiorna I_ref_A, in_position; in modalità non-cascata pilota anche il PWM */
void PlantControl_Step1kHz(PlantControlCtx* c, uint16_t target_adc, uint16_t y_adc);

/* Accessor default cfg (static const nel .c dedicato) */
const PlantControlCfg* PlantControl_BaseCfg(void);

/* Getter stato */
bool  PlantControl_IsInPosition(const PlantControlCtx* c);
float PlantControl_LastCommand(const PlantControlCtx* c);

/* Adapter per comandi in Ampere (es. calibrazioni open-loop) */
void PlantControl_ApplyAmpAdapter(void* user, float v); // funzione che viene utilizzata per poter comandare l'attuatore con una tensione affinchè questa venga mappata in dir/duty

/* Abilita/disabilita modalità cascata (true = anello corrente chiuso a 10 kHz) */
void PlantControl_EnableCascade(PlantControlCtx* c, bool en);

/* Variante: solo calcolo I_ref_A senza attuare PWM (sempre usata internamente) */
float PlantControl_Step1kHz_RefOnly(PlantControlCtx* c, uint16_t target_adc, uint16_t y_adc);

// Helper per controllare la corrente a LOOP APERTO
void PlantSetDutyDir(PlantControlCtx* c);
void PlantSetDutyDir_Sine10Hz(PlantControlCtx* c);
bool LimitCurrent_TRIPM1(PlantControlCtx* c);
/********************************************************************************************************************/

#endif /* INCLUDE_APP_PLANT_CONTROL_H_ */
