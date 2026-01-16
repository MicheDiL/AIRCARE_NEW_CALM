/*
 * pcm_tests.h
 *
 *  Created on: 28 nov 2025
 *      Author: mdilucchio
 *
 *      Prima di chiudere l’anello di corrente conviene “certificare” PCM.
 *
 *      Checklist operativa
 *
 *          1. Offset a ponte idle (baseline)
 *          Obiettivo: validare la baseline di PCM a ponte in coast (duty=0), e verificare che la stima d’offset (mediana a 10 kHz che fai in PRECHECK) sia OK
 *              Scenario: VMOT abilitata, PWM a 0% (coast).
 *              Cosa loggare: pcm1_lsb grezzo e i_pcm1_A filtrato (con Uart_TrySendCur e/o Uart_TrySendCurSigned_mA).
 *              Atteso: media pcm1_lsb ~ 2048 +/- ~20 LSB (+/-16 mV), i_pcm1_A ~ 0 A dopo la stima offset
 *
 *          2. Segno e coerenza con direzione
 *          Obiettivo: Verificare il segno della corrente misurata con duty costante e piccolo e direzione alternata: ogni blocco di t_hold_ms (default 1000 ms) il codice inverte il segno, tenendo il duty fissato
 *              Scenario: open-loop a bassa duty (5–10%), prima FWD poi REV.
 *              Atteso: i_pcm1_A >0 in direzione che aumenta ENC1, <0 nell’altra.
 *              Esito: se il segno è invertito, correggi la tua mappa dir_to_increase o ribalta il segno in conversione
 *
 *          3. Scala (gain) e linearità a bassa corrente
 *          Obiettivo: Eseguire una rampa triangolare di duty tra due limiti (p.es. 2% - 20%) a una velocità nota (p.es. 2 s per semiperiodo), in un’unica direzione elettrica. Serve per stimare in modo pulito il guadagno elettrico della catena (duty -> tensione ponte -> corrente su Rshunt) nella banda “quasi stazionaria” utile al PI di corrente.
 *              Scenario: sweep duty 0->20% (step piccoli, p.es. 1–2%), carico meccanico “libero”.
 *              Atteso: pendenza coerente con 2.0 V/A complessivi (G=100, Rshunt=20 mOhm).
 *              Regola d’oro già calcolata: 1 A ~ 2482 LSB, 1 mA ~ 2.48 LSB, offset a ~2048 LSB.
 *              Esito: errore di scala accettabile ±5…10% (attriti, back-EMF). Se fuori banda: ricontrolla Rshunt/gain o VREF ADC.
 *
 *          4. Confronto PCM vs ACM (sanity)
 *          Obiettivo: Eseguire una sequenza di gradini di duty (LOW -> HIGH -> LOW, ripetuta) in un’unica direzione elettrica, per misurare la risposta in corrente del plant
 *              Scenario: stesso sweep di cui sopra.
 *              Atteso: |ACM| ~ |PCM| entro +/-10…15% (ACM è “approx” e monodirezionale).
 *              Esito: se diverge molto, c’è un problema di catena IPROPI o di mappatura canali.
 *
 *          5. Rumore e filtraggio
 *          Obiettivo: Validare il rumore (jitter + ripple residuo) della catena di misura corrente PCM (INA240 -> ADC) a duty fisso.
 *              Scenario: duty costante bassa (5–10%) e poi media (30–40%).
 *              Cosa guardare: RMS del rumore su pcm1_lsb (grezzo) e su i_pcm1_A (filtrato).
 *              Atteso: filtro IIR (alpha ~ exp(-Ts/tau) con fc=2 kHz, fs=10 kHz -> alpha~0.73) abbassa jitter; banda residua “pulita”.
 *              Tuning: se ripple > utile, abbassa PCM_IIR_FC_HZ (p.es. 1 kHz) e ricava nuovo alpha una tantum in init.
 *              Implicazione: target di banda dell’anello corrente <= 1–2 kHz (il loop a 10 kHz è ok come sampling; il limite è il front-end).
 *
 *          6. Saturazioni e headroom
 *          Obiettivo: Tastare i limiti: clip ADC/INA, limite “soft” a I_soft_max e margine rispetto alla soglia di trip driver
 *              Atteso: il front-end satura a circa +/-0.825 A (1.65 V offset / 2.0 V/A).
 *              Test: aumentare duty con carico frenato (senza forzare lo stallo prolungato) e verificare “clipping” a pcm1_lsb~0/4095.
 *              Esito: se clip compare prima, controlla alimentazioni e il filtro RC; se non compare mai, sei limitato dal carico/motore (ok).
 *
 */

#ifndef INCLUDE_TESTS_PCM_TESTS_H_
#define INCLUDE_TESTS_PCM_TESTS_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
/* Frequenze note dal tuo progetto */
#ifndef PCM_TEST_FS_HZ
#define PCM_TEST_FS_HZ     10000u   /* Tick fast 10 kHz */
#endif
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef enum {
  PCMTEST_IDLE = 0,          /* nessun test attivo */
  PCMTEST_OFFSET_VERIFY,     /* baseline offset con duty=0 */
  PCMTEST_SIGN_CHECK,        /* segno corrente: alterna +/- small duty */
  PCMTEST_GAIN_SWEEP,        /* rampa 0->Dmax->0 per pendenza I(duty) */
  PCMTEST_STEP_RESPONSE,     /* step Dlo->Dhi */
  PCMTEST_NOISE_RMS,         /* duty fisso, misura RMS */
  PCMTEST_SATURATION_TRIP    /* aumenta duty fino a clip o I_soft_max */
} PcmTestMode;

typedef struct {
  /* Stimolo */
  float   duty_min;          /* 0..1 */
  float   duty_max;          /* 0..1 */
  uint16_t t_ramp_ms;        /* sweep up/down complessivo (GAIN_SWEEP) */
  uint16_t t_hold_ms;        /* hold per STEP/NOISE */
  int8_t  dir_sign;          /* +1 = dir ACT_DIR_FWD, -1 = ACT_DIR_REV */

  /* Limiti */
  float   i_soft_max_A;      /* es. 0.8..1.6 A per protezione test */
  uint16_t pcm_clip_lsb;     /* soglia clip: vicino a 0 o 4095, es. 100 LSB */

  /* Telemetria */
  uint16_t tlm_ms;           /* cadenza invio pacchetti test via UART (ms) */
} PcmTestCfg;

typedef struct {
  PcmTestMode mode;
  PcmTestCfg  cfg;

  /* stato interno */
  uint32_t t_ms;         /* contatore ms globale test */
  uint32_t t10k;         /* contatore tick 10k accumulati (per derive ms) */

  /* flag di esecuzione */
  bool     running;
  bool     done;

  /* accumuli statistici (NOISE_RMS) */
  float    acc;          /* somma i[k] */
  float    acc2;         /* somma i[k]^2 */
  uint32_t n;

  /* risultati NOISE_RMS */
  float    mean_A;             /* media i (A)   */
  float    rms_A;              /* rms i (A)     */

  /* SATURATION_TRIP */
  float    dprog;              /* duty progressivo */
  uint16_t step_ms;
  uint32_t last_step_ms;       /* ultimo ms usato per l’incremento */

  /* ultimi valori per telemetria/GUI */
  float    last_duty;    /* 0..1 */
  float    last_i_A;     /* corrente filtrata (PCM) */
  uint16_t last_pcm_lsb; /* grezzo */
  uint16_t last_acm_lsb; /* grezzo */

} PcmTestCtx;

/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/

/* API */
void PcmTest_Init (PcmTestCtx* t);
void PcmTest_Stop (PcmTestCtx* t);
void PcmTest_Start(PcmTestCtx* t, PcmTestMode m, const PcmTestCfg* cfg);

static inline bool PcmTest_IsRunning(const PcmTestCtx* t){ return t->running; }
static inline bool PcmTest_IsDone   (const PcmTestCtx* t){ return t->done;    }

/* Da chiamare a 10 kHz:
 *  - i_pcm_A: corrente precisa filtrata
 *  - pcm_lsb/acm_lsb: grezzi
 * Ritorna: duty [0..1] e segno direzione (+1/-1) da applicare open-loop.
 */
void PcmTest_Tick10kHz(PcmTestCtx* t,
                       float i_pcm_A,
                       uint16_t pcm_lsb,
                       uint16_t acm_lsb,
                       float* out_duty01,
                       int8_t* out_dir_sign);

/* Getter ausiliari per telemetria a 1 kHz */
static inline float   PcmTest_LastDuty01 (const PcmTestCtx* t){ return t->last_duty; }
static inline uint16_t PcmTest_LastPcmLsb(const PcmTestCtx* t){ return t->last_pcm_lsb; }
static inline uint16_t PcmTest_LastAcmLsb(const PcmTestCtx* t){ return t->last_acm_lsb; }
static inline float   PcmTest_LastIA     (const PcmTestCtx* t){ return t->last_i_A; }
static inline float    PcmTest_MeanA      (const PcmTestCtx* t){ return t->mean_A; }
static inline float    PcmTest_RmsA       (const PcmTestCtx* t){ return t->rms_A; }
/********************************************************************************************************************/

#endif /* INCLUDE_TESTS_PCM_TESTS_H_ */
