/*
 * pcm_tests.c
 *
 *  Created on: 28 nov 2025
 *      Author: mdilucchio
 */

#include "tests/pcm_tests.h"
#include "motor_driver/drv_etpwm.h"
#include "debug/uart.h"
#include <math.h>

/* ============= HELPERS =========*/
static inline float _clampf(float x,float lo,float hi){ return (x<lo)?lo:((x>hi)?hi:x); }

/* Generatore triangolare 0..1..0 in T ms */
static float sweep_tri_01(uint32_t t_ms, uint16_t T_ms){
  if (T_ms == 0) return 0.0f;
  uint32_t t = t_ms % T_ms;
  float ph = (float)t / (float)T_ms;    /* 0..1 */
  /* 0->1 (0..0.5), 1->0 (0.5..1) */
  return (ph < 0.5f) ? (ph*2.0f) : (2.0f - 2.0f*ph);
}

/* STEP orchestrazione: duty low per T/3, poi high per T/3, poi low T/3 */
static float step_01(uint32_t t_ms, uint16_t T_ms, float du_lo, float du_hi){
  if (T_ms == 0) return du_lo;
  uint32_t t = t_ms % T_ms;
  uint16_t seg = T_ms/3;
  if (t < seg) return du_lo;
  if (t < 2*seg) return du_hi;
  return du_lo;
}

/* ============ API ============ */
void PcmTest_Init(PcmTestCtx* t){
  t->mode = PCMTEST_IDLE;
  t->running = false;
  t->done = false;
  t->t_ms = 0;
  t->t10k = 0;
  t->acc = t->acc2 = 0.0f;
  t->n = 0;
  t->mean_A = 0.0f;
  t->rms_A  = 0.0f;
  t->last_duty = 0.0f;
  t->last_i_A = 0.0f;
  t->last_pcm_lsb = 0;
  t->last_acm_lsb = 0;
}

void PcmTest_Stop(PcmTestCtx* t){
  t->running = false;
  t->done    = true;
  t->mode    = PCMTEST_IDLE;
  t->last_duty = 0.0f;
}

void PcmTest_Start(PcmTestCtx* t, PcmTestMode m, const PcmTestCfg* cfg){
  t->mode = m;
  t->cfg  = *cfg;
  t->t_ms = 0;
  t->t10k = 0;
  t->acc = t->acc2 = 0.0f; t->n = 0;
  t->mean_A = 0.0f; t->rms_A = 0.0f;
  t->running = true;
  t->done = false;
  t->last_duty = 0.0f;
  t->step_ms = 50u;
  t->dprog = _clampf(cfg->duty_min, 0.0f, 1.0f);
  t->last_step_ms = 0xFFFFFFFFu;
}

void PcmTest_Tick10kHz(PcmTestCtx* t,
                       float i_pcm_A,
                       uint16_t pcm_lsb,
                       uint16_t acm_lsb,
                       float* out_duty01,
                       int8_t* out_dir_sign)
{
  if (!t->running){
      *out_duty01 = 0.0f;
      *out_dir_sign = +1;
      return;
  }

  /* timebase */
  t->t10k++;
  if ((t->t10k % (PCM_TEST_FS_HZ/1000u)) == 0) t->t_ms++; /* ogni 10 tick a 10kHz (ovvero ogni 1ms) aumenta t_ms */

  /* latch ultimi */
  t->last_i_A = i_pcm_A;
  t->last_pcm_lsb = pcm_lsb;
  t->last_acm_lsb = acm_lsb;

  /* default */
  float duty = 0.0f;
  const float d_min = _clampf(t->cfg.duty_min, 0.0f, 1.0f);
  const float d_max = _clampf(t->cfg.duty_max, 0.0f, 1.0f);
  int8_t dir  = (t->cfg.dir_sign >= 0) ? +1 : -1;

  switch (t->mode)
    {
      case PCMTEST_OFFSET_VERIFY:
        duty = 0.0f;
        if (t->t_ms >= t->cfg.t_hold_ms) t->done = true;
        break;

      case PCMTEST_SIGN_CHECK: {
        float d = (d_min > 0.0f) ? d_min : 0.05f; /* default 5% */
        uint16_t T = t->cfg.t_hold_ms ? t->cfg.t_hold_ms : 500;
        duty = d;
        dir  = ((t->t_ms / T) % 2u) ? -dir : +dir;
      } break;

      case PCMTEST_GAIN_SWEEP: {
        uint16_t T = t->cfg.t_ramp_ms ? t->cfg.t_ramp_ms : 2000;
        float u = sweep_tri_01(t->t_ms, T);
        duty = d_min + (d_max - d_min) * u;     // mappa la triangolare nell’intervallo [duty_min, duty_max]
      } break;

      case PCMTEST_STEP_RESPONSE: {
        uint16_t T = t->cfg.t_hold_ms ? t->cfg.t_hold_ms : 900;
        float dlo = d_min;
        float dhi = (d_max > d_min) ? d_max : (d_min + 0.1f);
        duty = step_01(t->t_ms, T, dlo, dhi);
      } break;

      case PCMTEST_NOISE_RMS:
        duty = (d_max > 0.0f) ? d_max : d_min;          // imponiamo un PWM costante per tenere la corrente in una condizione stazionaria

        // Con i campioni filtrati (i_pcm_A a 10 kHz) calcoliamo:
        // - media (bias residuo dopo la stima offset),
        // - RMS del rumore (deviazione standard, quindi “quanto frulla” il segnale al netto del bias)
        t->acc  += i_pcm_A;             // l’i_pcm_A che accumuliamo è già filtrato IIR quindi l’RMS è “post-filtro” e riflette la banda che realmente userai per il controllo
        t->acc2 += i_pcm_A * i_pcm_A;   // accumuliamo il quadrato della corrente filtrata
        t->n++;
        if (t->t_ms >= t->cfg.t_hold_ms) {      // quando t_ms supera t_hold_ms chiudiamo la misura

            if (t->n > 0u) {
              float mean = t->acc / (float)t->n;        // media
              float msq  = t->acc2 / (float)t->n;       // media dei quadrati
              float var  = msq - mean*mean;             // varianza
              if (var < 0.0f) var = 0.0f;               // guardia numerica
              t->mean_A = mean;                         // salva media
              t->rms_A  = sqrtf(var);                   // salva RMS: è la deviazione standard del segnale filtrato
            }
            t->done = true;
        }
        break;

      case PCMTEST_SATURATION_TRIP:
        /* duty progressivo con “edge” ogni 50 ms */
        duty = t->dprog;

          if ((t->t_ms != t->last_step_ms) && ((t->t_ms % t->step_ms) == 0u)) {
            t->dprog += 0.01f;                  /* +1% ogni 50 ms */
            if (t->dprog > d_max) t->dprog = d_max;
            t->last_step_ms = t->t_ms;          /* edge detector sul ms */
          }

      break;

      default:
        duty = 0.0f;
        t->done = true;
        break;
    }

    duty = _clampf(duty, 0.0f, 1.0f);       // safety clamp del duty
    t->last_duty = duty;
    *out_duty01  = duty;
    *out_dir_sign= dir;

    /* protezione corrente */
    if (fabsf(i_pcm_A) >= t->cfg.i_soft_max_A) t->done = true;

    /* conclude -> fermati */
    if (t->done){
      t->running  = false;
      t->last_duty= 0.0f;
      *out_duty01 = 0.0f;
    }
}
