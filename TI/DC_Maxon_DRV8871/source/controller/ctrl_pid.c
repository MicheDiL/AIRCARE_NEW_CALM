/*
 * ctrl_pid.c
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 *
 *      Implementazione “pura” del PID (P + I con anti-windup back-calculation + D filtrato)
 */

#include "controller/ctrl_pid.h"

/* Helpers conversione */
static float adcLSB_to_V(uint16_t adc) {
    return ((float)adc) * (ADC_VREF_V / ADC_FS_LSB);    // Converte una lettura ADC (0..4095) in Volt secondo la formula: LSB = V_adc*(4095/3.3)
}

void CtrlPid_Init(CtrlPidCtx* c, const CtrlPidCfg* cfg)
{
    // copia la config (*cfg) dentro il Ctx e azzera lo stato
    c->cfg = *cfg;
    CtrlPid_Reset(c);
}

void CtrlPid_Reset(CtrlPidCtx* c)
{
    c->st.d_filt        = 0.0f;
    c->st.e_prev_V      = 0.0f;
    c->st.y_prev_adc    = 0u;
}

float CtrlPid_Step(CtrlPidCtx* c, float e_V, uint16_t y_adc_cur)
{
    const float Ts   = c->cfg.Ts_s;
    const float invT = (c->cfg.TfD_s > 0.0f) ? (Ts / c->cfg.TfD_s) : 1.0f;

    float D_A;  // preparo il contributo derivativo

    // =========== RAMO DERIVATIVO FILTRATO =======================
    if (c->cfg.d_on_meas) {
        // --- D sulla misura: evita il derivative kick ---
        // y in Volt: converti l'ADC prima di differenziare
        float y_V  = adcLSB_to_V(y_adc_cur);
        float yp_V = adcLSB_to_V((uint16_t)c->st.y_prev_adc);
        float dydt = (yp_V - y_V) / Ts;            /* D = -dy/dt */

        c->st.d_filt += invT * (dydt - c->st.d_filt);   // filtro 1° ordine

        D_A = c->cfg.Kd_A * c->st.d_filt;

    } else {
        // --- D sull'errore: più reattivo ma soffre i salti di set-point ---
        float dedt = (e_V - c->st.e_prev_V) / Ts;
        c->st.d_filt += invT * (dedt - c->st.d_filt);   // filtro 1° ordine

        D_A = c->cfg.Kd_A * c->st.d_filt;
    }

    // ============ RAMO PROPORZIONALE ========================
    float P_A = c->cfg.Kp_A * e_V;

    // ===== Modalità PD (Ki=0 && Kaw=0) =====
    float I_cmd_A = P_A + D_A;                                   // nessun I
                                             // forza a zero
    c->st.e_prev_V   = e_V;
    c->st.y_prev_adc = (uint32_t)y_adc_cur;
    return I_cmd_A;


}
