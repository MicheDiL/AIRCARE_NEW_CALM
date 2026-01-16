/*
 * current_sense.h
 *
 *  Created on: 19 nov 2025
 *      Author: mdilucchio
 *
 *      Current_sense serve a portare a unità fisiche (Ampere) le due letture di corrente che ADC Group1 già acquisisce:
 *      - ACM = “approximate current” dal DRV8231A / IPROPI -> corrente senza segno (valore assoluto).
 *      - PCM = “precise current” da INA240 + Rshunt -> corrente bipolare (offset a metà scala, ~1.65 V).
 *
 *      In pratica, prende i LSB dell’ADC (0..4095) e li mappa in A usando i coefficienti elettrici reali della tua scheda (R, gain, offset)
 */

#ifndef INCLUDE_SAMPLING_ADC_CURRENT_SENSE_H_
#define INCLUDE_SAMPLING_ADC_CURRENT_SENSE_H_

/*************************************************** Include Files **************************************************/
#include "sampling_adc/drv_adc.h"   // per ADC_VREF_V / ADC_FS_LSB
#include <math.h>
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
/* === Parametri hardware (Channel_A) === */
#ifndef AIPROPI_UA_PER_A
#define AIPROPI_UA_PER_A    1500.0f      // DRV8231A tipico: 1500 microA/A
#endif

#ifndef RIPROPI_OHM
#define RIPROPI_OHM         1000.0f      // R152
#endif

#ifndef INA_GAIN_V_PER_V
#define INA_GAIN_V_PER_V    100.0f        // INA240A3 dal calm pcb (gain 100 V/V)
#endif

#ifndef RSHUNT_OHM
#define RSHUNT_OHM          0.02f        // 20 mOhm
#endif

#ifndef PCM_VOFFSET_V
#define PCM_VOFFSET_V       1.65f        // midsupply 3.3V/2
#endif

/* === Misura accurata di PCM ===*/
#ifndef PCM_OFFSET_NSAMPLES
#define PCM_OFFSET_NSAMPLES 2000        // 2000 campioni @10kHz = 200 ms
#endif

/* === IIR corrente PCM === */
#ifndef PCM_IIR_FS_HZ
#define PCM_IIR_FS_HZ       10000.0f    // loop fast 10 kHz
#endif

#ifndef PCM_IIR_FC_HZ
#define PCM_IIR_FC_HZ       2000.0f     // fc nominale 2 kHz
#endif

/* === Parametri per la conversioni TRIP ===*/
#ifndef TRIPM1_RTOP_OHM
#define TRIPM1_RTOP_OHM     100000.0f  // R33
#endif
#ifndef TRIPM1_RBOT_OHM
#define TRIPM1_RBOT_OHM     25500.0f  // R35
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef struct {
    float y;    /* stato del filtro IIR */
} PcmIirState;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
static inline float _clampf(float x, float lo, float hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}

/* === Helper per le conversioni delle CORRENTI  === */
static inline float acmLSB_to_A(uint16_t lsb){

    const float v = ( (float)lsb * (ADC_VREF_V / (float)ADC_FS_LSB) );      // LSB -> Volt
    return v / ((AIPROPI_UA_PER_A * 1e-6f) * RIPROPI_OHM);                  // I = V / (Aipropi * R)
}

/*static inline float pcmLSB_to_A(uint16_t lsb){
    // I = (Vout - Voff) / (G * Rshunt)
    const float v = ( (float)lsb * (ADC_VREF_V / (float)ADC_FS_LSB) );      // LSB -> Volt
    return (v - PCM_VOFFSET_V) / (INA_GAIN_V_PER_V * RSHUNT_OHM);
}
static inline float pcmLSB_to_A(uint16_t lsb, float voff_V){
    const float v = ( (float)lsb * (ADC_VREF_V / (float)ADC_FS_LSB) );
    return (v - voff_V) / (INA_GAIN_V_PER_V * RSHUNT_OHM);
}
// === Helper per il calcolo preciso dell'offset del sensore INA ===
static inline float pcmLSB_to_A_withOff(uint16_t lsb, float voff_V){
    const float v = ( (float)lsb * (ADC_VREF_V / (float)ADC_FS_LSB) );
    return (v - voff_V) / (INA_GAIN_V_PER_V * RSHUNT_OHM);
}*/

/* === Helper per le conversioni TRIP === */
static inline float tripLSB_to_VmotNode(uint16_t lsb){
    const float v_adc = (float)lsb * (ADC_VREF_V / (float)ADC_FS_LSB);
    const float Kup   = (TRIPM1_RTOP_OHM + TRIPM1_RBOT_OHM) / TRIPM1_RBOT_OHM; //  circa 4.922
    return v_adc * Kup;   // Volt su MOT1P dato che: Vtrip = Vadc = Vmot1p * (TRIPM1_RBOT_OHM/TRIPM1_R_TOP_OHM + TRIPM1_R_BOT_OHM) = Vmot1p * k_div con k_div = 1/Kup
}

/* ----------------------------------------------------------------------------
 * PCM -> A con IIR incorporato (1° ordine)
 *
 * Uso tipico @10kHz
 *
 * Parametri:
 *   lsb       : campione ADC 12-bit del canale PCM (0..4095)
 *   voff_V    : offset in Volt (baseline a ponte idle). Se NAN -> usa l’ultimo noto
 *   fc_hz     : frequenza di taglio del filtro (Hz). Se <=0 -> mantiene l’alpha corrente
 *   Alpha = exp(-Ts/tau) con tau = 1/(2pi fc). Ts = 1/PCM_IIR_FS_HZ
 */

/* calcolo alpha da fc (cut-frequency) e fs (sampling-frequency) */
static inline float iir_alpha_from_fc(const float fc, const float fs){
    const float Ts = 1.0f / fs;
    const float tau = 1.0f / (2.0f * M_PI * fc);
    return expf(-Ts / tau);
}

/*static inline float pcmLSB_to_A_IIR(uint16_t lsb, float voff_V)
{
    // Stato interno del filtro / cache parametri
    float s_alpha = iir_alpha_from_fc(PCM_IIR_FC_HZ, PCM_IIR_FS_HZ);   // default ~2 kHz @ 10 kHz: POSSIBILE MIGLIORAMENTE: evita di calcolarlo ad ogni campione

    // LSB -> Volt (ADC)
    float v_adc = ((float)lsb) * (ADC_VREF_V / (float)ADC_FS_LSB);
    // Volt -> Ampere (rimozione offset + guadagno INA * Rshunt)
    float i_raw = (v_adc - voff_V) / (INA_GAIN_V_PER_V * RSHUNT_OHM);

    // IIR 1° ordine: y[k] = a*y[k-1] + (1-a)*x[k]
    static float i_filt = 0.0f;
    i_filt = s_alpha * i_filt + (1.0f - s_alpha) * i_raw;
    return i_filt;
}*/

static inline float pcmLSB_to_A_IIR(uint16_t lsb, float voff_V, float alpha, PcmIirState* s)
{
    const float v_adc = ((float)lsb) * (ADC_VREF_V / (float)ADC_FS_LSB);
    const float i_raw = (v_adc - voff_V) / (INA_GAIN_V_PER_V * RSHUNT_OHM);
    const float a     = _clampf(alpha, 0.0f, 0.999999f);
    s->y = i_raw; //a*s->y + (1.0f - a)*i_raw
    return s->y;
}
/********************************************************************************************************************/

#endif /* INCLUDE_SAMPLING_ADC_CURRENT_SENSE_H_ */
