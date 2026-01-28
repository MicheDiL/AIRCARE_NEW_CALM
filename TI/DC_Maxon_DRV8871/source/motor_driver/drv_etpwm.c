/*
 * drv_etpwm.c
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 */

#include "motor_driver/drv_etpwm.h"

static volatile DecayMode s_mode = DECAY_BRAKE;  // default
/* ================= Helpers interni ================= */

static inline float clamp01(float x)
{
    if (x < -1.0f/*0.0f*/) return -1.0f/*0.0f*/;
    if (x > 1.0f) return 1.0f;
    return x;
}

// mappa duty -> CMP per PWM centrato e attivo-alto:
// - Con le AQ impostate (SET in salita @CMP, CLEAR in discesa @CMP), più piccolo è CMP -> più grande è il duty.
static inline uint16_t duty_to_cmp(float duty01, uint16_t prd)
{
    float d = clamp01(duty01);
    uint32_t cmp = (uint32_t)((1.0f - d) * (float)prd);

    /* Evita esattamente 0 o PRD quando non sei a 100/0% per ridurre glitch */
    if (cmp == 0u && d < 1.0f)   cmp = 1u;
    if (cmp >= prd && d > 0.0f)  cmp = (uint32_t)prd - 1u;

    return (uint16_t)cmp;
}

static inline uint16 calc_TBPRD(void){
    uint32 tbclk = VCLK3_HZ; // / (HSP*CLKDIV)=1
    uint32 prd   = tbclk / (2u * ETPWM_FREQ_HZ); // 75e6/(2*50e3)=750
    if (prd == 0u) prd = 1u;
    if (prd > 0xFFFFu) prd = 0xFFFFu;            // saturazione a 16 bit
    return (uint16)prd;
}

/* ================= API ================= */

void DrvPwm_Init()
{
    // 1) Divisori TBCLK = VCLK3/(HSP*CLK) = 75 MHz
    etpwmSetClkDiv(etpwmREG1, CLKDIV, HSPCLKDIV);

    // 2) Conteggio Up-Down
    etpwmSetCounterMode(etpwmREG1, CounterMode_UpDown);

    // 3) Periodo (TBPRD) per 50 kHz
    uint16_t prd = calc_TBPRD();
    etpwmSetTimebasePeriod(etpwmREG1, prd);
    etpwmSetCount(etpwmREG1, 0);

    // 4) Shadow load su CMPA/CMPB (carica a CTR=Zero e/o PRD, a scelta)
    etpwmEnableCmpAShadowMode(etpwmREG1, LoadMode_CtrEqZero);
    etpwmEnableCmpBShadowMode(etpwmREG1, LoadMode_CtrEqZero);

    // 5) Action Qualifier per PWM centrato attorno al centro periodo; la larghezza d’impulso è inversamente proporzionale a CMP
    // - CMP piccolo: CAU avviene presto in salita, CAD avviene tardi in discesa => duty grande (circa 100%)
    // - CMP grande (vicino a TBPRD): CAU/CAD molto vicini al bordo => duty piccolo (circa 0%)
    etpwmActionQualConfig_t aqA = {
      .CtrEqZero_Action     = ActionQual_Disabled,
      .CtrEqPeriod_Action   = ActionQual_Disabled,
      .CtrEqCmpAUp_Action   = ActionQual_Set,     // CAU
      .CtrEqCmpADown_Action = ActionQual_Clear,   // CAD
      .CtrEqCmpBUp_Action   = ActionQual_Disabled,
      .CtrEqCmpBDown_Action = ActionQual_Disabled
    };
    etpwmSetActionQualPwmA(etpwmREG1, aqA);

    etpwmActionQualConfig_t aqB = {
      .CtrEqZero_Action     = ActionQual_Disabled,
      .CtrEqPeriod_Action   = ActionQual_Disabled,
      .CtrEqCmpBUp_Action   = ActionQual_Set,     // CBU
      .CtrEqCmpBDown_Action = ActionQual_Clear,   // CBD
      .CtrEqCmpAUp_Action   = ActionQual_Disabled,
      .CtrEqCmpADown_Action = ActionQual_Disabled
    };
    etpwmSetActionQualPwmB(etpwmREG1, aqB);
    // entrambi i canali producono PWM alto per la quota di periodo determinata da CMP

    ////  SOCA: abilita, sorgente = CTR=PRD (vertice metà-TON), ogni evento
    etpwmEnableSOCA(etpwmREG1, CTR_PRD, EventPeriod_FirstEvent);       // SOCA @ PRD

    // 6) Avvia TBCLK dei moduli ePWM
    etpwmStartTBCLK();

    // Duty iniziali a zero
    etpwmSetCmpA(etpwmREG1, duty_to_cmp(0.0f, prd));
    etpwmSetCmpB(etpwmREG1, duty_to_cmp(0.0f, prd));
}

void DrvPwm_SetMode(DecayMode m){
    s_mode = m;
}

DecayMode DrvPwm_GetMode(void){
    return s_mode;
}

void DrvPwm_SetDutyRawAB(float dutyA01, float dutyB01)
{
    uint16_t prd = etpwmREG1->TBPRD;
    etpwmSetCmpA(etpwmREG1, duty_to_cmp(dutyA01, prd));
    etpwmSetCmpB(etpwmREG1, duty_to_cmp(dutyB01, prd));
}

void DrvPwm_Coast(void)
{
    DrvPwm_SetDutyRawAB(0.0f, 0.0f); // A=0, B=0
}

void DrvPwm_Brake(void)
{
    DrvPwm_SetDutyRawAB(1.0f, 1.0f); // A=1, B=1
}

void DrvPwm_Drive(ActDir dir, float duty01)
{
    float d = clamp01(duty01);    // clamp di sicurezza sul dutycycle del segnale PWM

    if (s_mode == DECAY_COAST) {
       /* Schema attuale: drive-coast
        * FWD: IN1=0 fisso, IN2=PWM alto(d)
        * REV: IN2=0 fisso, IN1=PWM alto(d)
        * dove:
        * EPWMA = IN1
        * EPWMB = IN2
        */
       if (dir == ACT_DIR_FWD) {
           DrvPwm_SetDutyRawAB(0.0f, /*duty01*/d);
       } else {
           DrvPwm_SetDutyRawAB(/*duty01*/d, 0.0f);
       }
   } else { // DECAY_BRAKE (schema TI)
       /* Serve una PWM attiva-bassa su una linea; siccome l’AQ genera attivo-alto,
        * il codice usa inv = 1-d per ottenere la complementare.
        * FWD: IN1=1 fisso, IN2=PWM(inv) -> alterni 10 (drive) e 11 (brake).
        * REV: speculare.
        */
       float inv = 1.0f - fabs(/*duty01*/d);    // genera il "PWM attivo-basso" usando il tuo PWM attivo-alto
       if (dir == ACT_DIR_FWD) {
           DrvPwm_SetDutyRawAB(1.0f, inv);    // IN1=1 fisso, IN2=PWM alto(1-d)
       } else {
           DrvPwm_SetDutyRawAB(inv, 1.0f);    // IN1=PWM alto(1-d), IN2=1 fisso
       }
   }
}
