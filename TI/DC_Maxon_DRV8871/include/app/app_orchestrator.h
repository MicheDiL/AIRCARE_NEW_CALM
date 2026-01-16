/*
 * app_orchestrator.h
 *
 *  Created on: 14 nov 2025
 *      Author: mdilucchio
 *
 *      Governa gli stati e chiama di volta in volta board_pwr, app_calibrator, plant_control, sp_gen, drv_adc, drv_etpwm, uart
 */

#ifndef INCLUDE_APP_APP_ORCHESTRATOR_H_
#define INCLUDE_APP_APP_ORCHESTRATOR_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "control_timer/drv_rti.h"
#include "power_up_ups/board_power.h"
#include "plant/plant_control.h"    // fa policy attuazione
#include "setpoint/sp_gen.h"
#include "sampling_adc/drv_adc.h"
#include "sampling_adc/current_sense.h"
#include "motor_driver/drv_etpwm.h"
#include "debug/uart_to_qt.h"           //#include "debug/uart.h"
#include "tuning/tuning_store.h"
#include "leds/signaling_leds.h"
#include "button/pushbutton.h"

#include "tests/pcm_tests.h"
#include "controller/current_ctrl.h"
#include "setpoint/sp_CurGen.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
#ifndef I_SOFT_MAX_A
#define I_SOFT_MAX_A   0.4f     // limite “soft” sotto Itrip driver (2A)
#endif

#ifndef RMOTOR_OHM
#define RMOTOR_OHM     7.29f     // Maxon DCX12L
#endif

// Valori delle resistenze dei partitori di tensione per lettura ADC
#define R33             100.0f
#define R35             25.5f
#define R50             1.0f
#define R49             2.2f
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef enum {      // definisce un insieme di costanti numeriche con un alias di tipo AppState
  APP_BOOT = 0,
  APP_POWER_PRECHECK,               // in questo stato vengono fatti i seguenti check: ALIMENTAZIONE DI SISTEMA STABILE (VSYS), DRIVER MOTORE ALIMENTATO (VMOT), SENSORI DI POSIZIONE ALIMENTATI (5VMOT)
  APP_CALIBRATING,
  APP_READY,
  APP_TESTING,            /* <<< NEW */
  APP_CONTROL_ACTIVE,
  APP_FAULT,
  APP_OVERCURRENT,
  APP_SHUTDOWN
} AppState;

typedef enum {
  APPF_NONE            = 0,         // 0b0000 in binario che segnale: nessun fault
  APPF_PWR_DEGRADE     = (1u<<0),   // 0b0001 in binario che segnala: !power_ok (nIPREMOVED=0 o nUPS_ALERT=1)
  APPF_ADC_STALE       = (1u<<1),   // 0b0010 in binario che segnala: mancato sample per N ms
  APPF_USER_SHUTDOWN   = (1u<<2),   // 0b0100 in binario che segnala: comando software
  // …in futuro: driver ST fault, overtemp, ecc.
  APPF_OVERCURRENT      = (1u<<3),
} AppFaultMask;

/* === LED patterns orchestrati === */
typedef enum {
  LEDPAT_OFF = 0,
  LEDPAT_SOLID_G,                   // verde fisso
  LEDPAT_SOLID_R,                   // rosso fisso
  LEDPAT_SOLID_B,                   // blu fisso
  LEDPAT_YELLOW_BLINK_SLOW,         // giallo (R+G) blink lento
  LEDPAT_BLUE_BLINK_SLOW,           // blu blink lento
  LEDPAT_GREEN_HEARTBEAT,           // doppio battito verde ogni 2s
  LEDPAT_RED_DOUBLE_BLINK,          // doppio blink rosso (fault)
} LedPattern;

typedef enum {
  IREF_SRC_ON_BOARD = 0,
  IREF_SRC_FROM_POSITION_LOOP
} IRefSource;

typedef struct {
  // Sub-contesti riusati
  BoardPwrCtx     pwr;
  PlantControlCtx ctrl;
  SpGen           spg;
  BtnCtx          btn;

  /* Acquisizioni ADC */
  AdcPos          adc_pos;              // 1 kHz: ENC1
  AdcCurrents     adc_currents;         // 10 kHz: ACM/PCM/TRIP

  /* Tuning da Qt */
  UartRxCtx s_rx;
  TuningStore tune;

  /* === NEW: stato IIR + alpha precomputata === */
  PcmIirState     pcm_iir;
  float           pcm_alpha;   // precomputata da PCM_IIR_FC_HZ/PCM_IIR_FS_HZ

  /* === NEW: test PCM === */
  PcmTestCtx  pcm_test;

  /* === NEW: Loop di Corrente === */
  CurPiCtx     cur_pi;         /* PI di corrente */
  IRef          iref;        /* generatore triangolare a gradini */

  /* Correnti convertite / diagnostica */
  float           i_pcm1_A;           // corrente precise (INA240)
  float           i_acm1_A;           // corrente approx (IPROPI)
  float           i_ref1_A;           // riferimento corrente (per futuro PI) */
  float           i_meas1_A_last;     // NEW: ultimo campione per telemetria a 1 kHz

  /* Offset PCM runtime */
  float           pcm_offset_lsb;
  float           pcm_offset_v;

  int general_cnt;

  struct {
     uint32_t acc;
     uint32_t n;
     bool     done;
  } pcm_off_cal;

  // Stato orchestratore
  AppState        state;
  uint32_t        faults;             // bitmask di AppFaultMask
  uint16_t        adc_stale_ms;       // contatore per ADC watchdog

  struct{
     LedPattern   pat;
     uint16_t     t_ms;              // contatore millisecondi pattern corrente
     uint8_t      pulse_idx;         // supporto per heartbeat/doppi blink
  }leds;

} AppOrch;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void AppOrch_Init(AppOrch* a,
                  const CtrlPidCfg* pidCfg,
                  const PlantControlCfg* plantCfg,
                  const SpGenCfg* spgCfg);   // carica cfg e resetta stati

void AppOrch_Tick1kHz(AppOrch* a);           // da chiamare ogni ms

void AppOrch_Tick10kHz(AppOrch* a);         // NEW

// Utility
AppState AppOrch_State(const AppOrch* a);
uint32_t AppOrch_Faults(const AppOrch* a);
void     AppOrch_RequestShutdown(AppOrch* a); // alza APPF_USER_SHUTDOWN
/********************************************************************************************************************/

#endif /* INCLUDE_APP_APP_ORCHESTRATOR_H_ */
