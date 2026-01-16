/*
 * drv_adc.c
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 */


#include "sampling_adc/drv_adc.h"

/* =================== Stato interno =================== */

static volatile uint32_t s_acc_enc1 = 0;     /* somma POS */
static volatile uint32_t s_acc_acm1 = 0;     /* somma ACM */
static volatile uint32_t s_acc_pcm1 = 0;     /* somma PCM */
static volatile uint32_t s_acc_trip1 = 0;

static volatile uint16_t s_n_enc1 = 0;       /* contatore per POS (fino a 50) */
static volatile uint16_t s_n_cur = 0;       /* contatore per correnti (fino a 5) */

static volatile uint16_t s_last_enc1 = 0;    /* ultimo POS mediato @1k */
static volatile uint16_t s_last_acm1 = 0;    /* ultimo ACM mediato @10k */
static volatile uint16_t s_last_pcm1 = 0;    /* ultimo PCM mediato @10k */
static volatile uint16_t s_last_trip1 = 0;

static volatile uint8_t  s_enc1_ready = 0;   /* nuovo POS @1k pronto */
static volatile uint8_t  s_cur_ready = 0;   /* nuove correnti @10k pronte */

/* ====================================================== */

/* ============================ API =================================== */

void DrvAdc_Init()
{
    ////////////////////// ADC //////////////////////
    adcInit();                                        /* HALCoGen setup */
    adcEnableNotification(adcREG1, adcGROUP1);        /* EOC interrupt */
    adcStartConversion(adcREG1, adcGROUP1);           /* parte con trigger HW (EPWM1 SOCA) */
}

void DrvAdc_Pos_Init(AdcPos* c){
    //NEW Moving Average posizione
    uint8_t p2 = c->pos_ma_pow2 = 3u;   // N = 2^3 = 8
    uint16_t i;

    if (p2 > POS_MA_POW2_MAX) p2 = POS_MA_POW2_MAX;
    c->pos_ma_pow2 = p2;

    uint16_t N = (uint16_t)(1u << p2);
    c->pos_mask = (uint16_t)(N - 1u);
    c->pos_acc  = 0u;
    c->pos_idx  = 0u;
    c->pos_fill = 0u;
    for (i=0; i<N; ++i) c->pos_buf[i] = 0u;
}

void DrvAdc_Pos_Reset(AdcPos* c){

    //NEW Moving Average posizione
    uint16_t i;
    uint16_t N = (uint16_t)(1u << c->pos_ma_pow2);
    c->pos_acc  = 0u;
    c->pos_idx  = 0u;
    c->pos_fill = 0u;
    for (i=0; i<N; ++i) c->pos_buf[i] = 0u;
}

// restituisce solo ENC1 e consuma il set
bool DrvAdc_ReadEnc_1kHz(AdcPos* out)
{
    if (!out) return false;

    if (s_enc1_ready) {
        out->enc1_lsb = s_last_enc1;
        s_enc1_ready  = 0u;                              // consume

        //NEW: Moving Average su posizione
        /*uint16_t N      = (uint16_t)(1u << out->pos_ma_pow2);       // finestra di 2^c->cfg.pos_ma_pow2) campioni
        uint16_t old    = out->pos_buf[out->pos_idx];               // pos_buf è il ring. L’indice pos_idx punta alla cella da sovrascrivere (quindi è anche il campione “più vecchio” che esce dalla finestra)
        out->pos_buf[out->pos_idx] = s_last_enc1;                   // Scriviamo il nuovo campione nel ring
        out->pos_idx = (out->pos_idx + 1u) & out->pos_mask;         // wrap-around modulo N: quando l’indice raggiunge N, riparte da 0

        out->pos_acc += (uint32_t)s_last_enc1;                      // aggiungi il nuovo campione alla somma cumulata
        out->pos_acc -= (uint32_t)old;                              // togliamo old dalla somma cumulata

        if (out->pos_fill < N) out->pos_fill++;                     // pos_fill conta quanti campioni validi abbiamo già dentro il buffer

        if (out->pos_fill < N) {
            // start-up: finché non hai N campioni, fai la media con il conteggio reale pos_fill. Così la media è corretta anche quando la finestra non è piena => evitare offset iniziale
            out->enc1_lsb = (uint16_t)(out->pos_acc / out->pos_fill);
        } else {
            // quando sei a regime (pos_fill == N): divisione per 2^p == right shift
            out->enc1_lsb = (uint16_t)(out->pos_acc >> out->pos_ma_pow2);
        }*/

        return true;
    }

    return false;
}

bool DrvAdc_ReadCurr_10kHz(AdcCurrents* out){
    if (!out) return false;
    if (s_cur_ready) {
        out->acm1_lsb = s_last_acm1;
        out->pcm1_lsb = s_last_pcm1;
        out->trip1_lsb = s_last_trip1;
        s_cur_ready   = 0;
        return true;
    }
    return false;
}


/* Peek */
uint16_t DrvAdc_PeekLast_Pcm1(void) { return s_last_pcm1; }
uint16_t DrvAdc_PeekLast_Enc1(void) { return s_last_enc1; }
uint16_t DrvAdc_PeekLast_Acm1(void) { return s_last_acm1; }

/* ============================ ISR =================================== */
/* ISR di fine conversione Group1: legge TUTTE le conversioni del burst */

void adcNotification(adcBASE_t *adc, uint32 group)
{
    if ((adc != adcREG1) || (group != adcGROUP1)) return;

    adcData_t d[ADC_CH_COUNT];
    uint32_t  count = adcGetData(adcREG1, adcGROUP1, d);   // legge un campione (12-bit)


    // Assumiamo ordine dei canali = ordine configurato in HALCoGen
    if (count >= ADC_CH_COUNT) {

        // accumula
        s_acc_pcm1   += (uint32_t)d[ADC_CH_PCM].value;
        s_acc_enc1   += (uint32_t)d[ADC_CH_ENC1].value;
        s_acc_acm1   += (uint32_t)d[ADC_CH_ACM].value;
        s_acc_trip1  += (uint32_t)d[ADC_CH_TRIP].value;   // accumula TRIP


        // ogni OSR_ENC1 (50 campioni) pubblica la media e alza un “new ready”
        if ( ++s_n_enc1 >= OSR_ENC1 ) {
            // media @1k
            uint32_t enc1 = s_acc_enc1 / OSR_ENC1;
            if (enc1 > ADC_FS_LSB) enc1 = ADC_FS_LSB;
            s_last_enc1 = (uint16_t)enc1;
            s_acc_enc1 = 0;
            s_n_enc1 = 0;
            s_enc1_ready = 1;
        }

        // ogni OSR_CUR (5 campioni) pubblica la media e alza un “new ready”
        if (++s_n_cur >= OSR_CURR) {
            // media @10k
            uint32_t acm1 = s_acc_acm1 / OSR_CURR;
            uint32_t pcm1 = s_acc_pcm1 / OSR_CURR;
            uint32_t trip1 = s_acc_trip1 / OSR_CURR;
            if (acm1 > ADC_FS_LSB) acm1 = ADC_FS_LSB;
            if (pcm1 > ADC_FS_LSB) pcm1 = ADC_FS_LSB;
            if (trip1 > ADC_FS_LSB) trip1 = ADC_FS_LSB;
            s_last_acm1 = (uint16_t)acm1;
            s_last_pcm1 = (uint16_t)pcm1;
            s_last_trip1 = (uint16_t)trip1;
            s_acc_acm1 = 0;
            s_acc_pcm1 = 0;
            s_acc_trip1 = 0;
            s_n_cur = 0;
            s_cur_ready = 1;
        }
    }

}

// NEW Moving Average posizione
void PlantControl_SetPosMaPow2(AdcPos* c, uint8_t p2)
{
    uint16_t i;
    if (p2 > POS_MA_POW2_MAX) p2 = POS_MA_POW2_MAX;
    c->pos_ma_pow2 = p2;

    /* reinit finestra mantenendo stato coerente */
    uint16_t N = (uint16_t)(1u << c->pos_ma_pow2);
    c->pos_mask = (uint16_t)(N - 1u);
    c->pos_acc  = 0u;
    c->pos_idx  = 0u;
    c->pos_fill = 0u;
    for (i=0; i<N; ++i) c->pos_buf[i] = 0u;
}
