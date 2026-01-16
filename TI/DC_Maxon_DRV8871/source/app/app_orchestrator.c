/*
 * app_orchestrator.c
 *
 *  Created on: 14 nov 2025
 *      Author: mdilucchio
 */

#include "app/app_orchestrator.h"

/* ======================================================== Helpers ======================================================== */
static inline void go_safe_coast(void){ DrvPwm_Coast(); }

static inline void _led_rgb(bool r, bool g, bool b){
  Led_Set(&LED_RED, r);
  Led_Set(&LED_GRN, g);
  Led_Set(&LED_BLU, b);
}
static inline void _led_all_off(void){ _led_rgb(false,false,false); }

/* === Mapping stato/fault -> pattern LED ===
   Regole:
   - FAULT          -> rosso doppio blink
   - POWER PRECHECK -> blu blink lento
   - CALIBRATING    -> giallo (R+G) blink lento
   - READY          -> verde fisso
   - ACTIVE         -> heartbeat verde
   - SHUTDOWN       -> rosso fisso
   - BOOT           -> blu fisso breve
*/

static LedPattern map_led_pattern(const AppOrch* a)
{
  if (a->state == APP_SHUTDOWN)             return LEDPAT_SOLID_R;
  if (a->state == APP_FAULT)                return LEDPAT_RED_DOUBLE_BLINK;
  if(a->state == APP_OVERCURRENT)           return LEDPAT_SOLID_R;

  switch (a->state) {
    //case APP_BOOT:                          return LEDPAT_SOLID_B;
    case APP_POWER_PRECHECK:                return LEDPAT_BLUE_BLINK_SLOW;
    case APP_TESTING:                       return LEDPAT_SOLID_B;
    case APP_CALIBRATING:                   return LEDPAT_YELLOW_BLINK_SLOW;
    case APP_READY:                         return LEDPAT_SOLID_G;
    case APP_CONTROL_ACTIVE:                return LEDPAT_GREEN_HEARTBEAT;
    default:                                return LEDPAT_OFF;
  }
}

static void leds_run_1kHz(AppOrch* a)
{
  LedPattern want = map_led_pattern(a);

  /* Cambio pattern -> reset timer locale */
  if (want != a->leds.pat){
    a->leds.pat = want;
    a->leds.t_ms = 0;
    a->leds.pulse_idx = 0;
    _led_all_off();
  } else {
    a->leds.t_ms++;
  }

  switch (a->leds.pat)
  {
    case LEDPAT_OFF:
      _led_all_off();
      break;

    case LEDPAT_SOLID_G:
      _led_rgb(false,true,false);
      break;

    case LEDPAT_SOLID_R:
      _led_rgb(true,false,false);
      break;

    case LEDPAT_SOLID_B:
      _led_rgb(false,false,true);
      break;

    case LEDPAT_YELLOW_BLINK_SLOW: {
      /* Periodo 1000 ms, ON 200 ms */
      uint16_t t = a->leds.t_ms % 1000;
      bool on = (t < 200);
      _led_rgb(on,on,false);        // Giallo = R+G
    } break;

    case LEDPAT_BLUE_BLINK_SLOW: {
      uint16_t t = a->leds.t_ms % 1000;
      bool on = (t < 200);
      _led_rgb(false,false,on);
    } break;

    case LEDPAT_GREEN_HEARTBEAT: {
      /* Due impulsi brevi ogni 2000 ms: 100ms ON, 100ms OFF, 100ms ON, poi pausa */
      uint16_t t = a->leds.t_ms % 1000;
      bool on = ( (t < 100) || (t >= 200 && t < 300) );
      _led_rgb(false,on,false);
    } break;

    case LEDPAT_RED_DOUBLE_BLINK: {
      /* Fault: doppio blink rosso ogni 1000 ms: 100 ON, 100 OFF, 100 ON, 700 OFF */
      uint16_t t = a->leds.t_ms % 1000;
      bool on = ( (t < 100) || (t >= 200 && t < 300) );
      _led_rgb(on,false,false);
    } break;

    default:
      _led_all_off();
      break;
  }
}

/* ===================== NEW: Test launcher via multi-click ===================== */
static void _start_test_from_clicks(AppOrch* a, uint8_t nclicks)
{

    switch (nclicks){
      case 1:  // OFFSET_VERIFY: duty=0, misura baseline
          IRef_Init(&a->iref);         // ripristina i limiti: ±0.40 A, step 0.04 A, 5 s per step
          IRef_Start(&a->iref);        // attiva il generatore
          a->i_ref1_A = IRef_GetA(&a->iref);
          a->state    = APP_TESTING;
       break;

      default:
          break;
    }

}

static inline ActDir invert_dir(ActDir d){
    return (d==ACT_DIR_FWD)?ACT_DIR_REV:ACT_DIR_FWD;
}

static inline float clamp(float x, float lo, float hi)
{ return (x<lo)?lo:((x>hi)?hi:x); }
/* ========================================================================================================================== */

void AppOrch_Init(AppOrch* a,
                  const CtrlPidCfg* pidCfg,
                  const PlantControlCfg* plantCfg,
                  const SpGenCfg* spgCfg)
{
    // ==== Inizializza Led =====
    Led_Init();

    // ==== Inizializza Power Chain =====
    BoardPwr_Init(&a->pwr);                                     // Boot: VMOTEN basso assicurato da BoardPwr_Init()

    // ==== Inizializza Pwm =====
    etpwmInit();                                                // ETPWM1: 50 kHz, SOCA @ PRD
    etpwmStopTBCLK();                                           // equivalente a mettere a 0 il bit TBCLKSYNC (PINMMR)
    DrvPwm_Init();                                              // Imposta Up-Down 50 kHz, shadow su CMPA/B, AQ (SET@CU, CLEAR@CD), abilita SOCA@PRD, riavvia TBCLK

    // ==== Inizializza Timer 1kHz =====
    DrvRti_Init_1kHz();                                         // Configura l’RTI Compare0 a 1 ms, abilita l’IRQ e avvia il contatore

    // ==== Inizializza Timer 10kHz =====
    DrvRtiFast_Init_10kHz();

    // ==== Inizializza ADC =====
    DrvAdc_Init();                                              // Abilita ADC1 Group1, notifica EOC e start conversion sotto trigger ePWM1 SOCA (50 kHz)

    // === NEW Moving Average ===
    DrvAdc_Pos_Init(&a->adc_pos);

    // ==== Inizializza Controllore PID =====
    PlantControl_Init(&a->ctrl, pidCfg, plantCfg);

    // ==== Inizializza il Generatore di SetPoint automatici per POSIZIONE =====
    SpGen_Init(&a->spg, spgCfg ? spgCfg : SpGen_DefaultCfg());

    // Abilita modalità cascata: PWM verrà comandato dall’anello corrente a 10 kHz
    //PlantControl_EnableCascade(&a->ctrl, true);

    // ==== Inizializza Comunicazione Seriale su SCI3 =====
    DrvSci_Init();                                              // prepara SCI3 e abilita TX RX IRQ

    // Telemtria su Python
    //Uart_Init(20u, 20u);

    // Telemtria su Qt GUI
    UartTelemetry_Init(20);     // TX 50Hz                         // pacchetti ogni 20 ms (50 Hz) sia per pos che per cur
    UartRx_Init(&a->s_rx);      // RX
    TuningStore_Init(&a->tune); // tuning da Qt

    // ==== Inizializza Bottone =====
    Btn_Init(&a->btn, NULL);                                    // usa configurazione di default per il bottone

    // === NEW:Inizializza filtro IIR per PCM ===
    a->pcm_iir.y  = 0.0f;
    a->pcm_alpha  = iir_alpha_from_fc(PCM_IIR_FC_HZ, PCM_IIR_FS_HZ);

    // === NEW: Inzializza i test per PCM ===
    PcmTest_Init(&a->pcm_test);

    // NEW: Inizializza Iref ===
    CurPi_Init(&a->cur_pi, CurPi_DefaultCfg());
    IRef_Init(&a->iref);
    a->i_ref1_A = 0.0f;

    a->state      = APP_BOOT;
    a->faults     = APPF_NONE;
}

void AppOrch_Tick1kHz(AppOrch* a)
{
      /* =================================================================================
                                    Time-based services
         =================================================================================*/

      // ======= Check sulla Power-Chain =========
      BoardPwr_Tick1kHz(&a->pwr);
      if(a->pwr.fault_latched){
          a->faults |= APPF_PWR_DEGRADE;
      }

      // ========= Check sul ADC ============
      bool got_pos = DrvAdc_ReadEnc_1kHz(&a->adc_pos);   // <-- nuova API POS 1kHz

      if (!got_pos) {
         if (a->adc_stale_ms < 1000){
             a->adc_stale_ms++;
         }
      }else{
         a->adc_stale_ms = 0;
      }

      if (a->adc_stale_ms > 50) {                               // >50 ms senza sample -> fault
          a->faults |= APPF_ADC_STALE;                          // attivia il fault APPF_ADC_STALE facendo l'OR bitwise
      }

      SpGen_Tick1ms(&a->spg);

      // ======== Stato dei led =========
      leds_run_1kHz(a);

      // ======= Check sul bottone =========
      Btn_Tick1kHz(&a->btn);

      /* Long-press -> spegnimento immediato e passaggio a SHUTDOWN */
      if (Btn_LongPressLatched(&a->btn)) {
            // Best effort: porta l’attuatore in stato sicuro prima di alzare Harakiri (già alzato dal modulo)
            a->state = APP_SHUTDOWN;
      }
      /* Eventi click multipli (facoltativi: telemetria/azioni) */
      uint8_t nclicks = 0;
      if (Btn_PopClickBurst(&a->btn, &nclicks)) {
          _start_test_from_clicks(a, nclicks);
      }

      /*===================================================================================*/

      /* =================================================================================
                                  Analisi su eventuali Faults
         =================================================================================*/

      if(((a->faults & APPF_PWR_DEGRADE) /*|| (a->faults & APPF_ADC_STALE)*/)){
          a->state = APP_FAULT;
      }else if((a->faults & APPF_OVERCURRENT)){
          a->state = APP_OVERCURRENT;
      }else{
          a->faults & APPF_NONE;
          a->state = a->state;
      }

      /* =================================================================================
                                         State Machine
         =================================================================================*/
      switch (a->state)
      {
            case APP_BOOT:

                a->leds.pat       = LEDPAT_OFF;
                a->leds.t_ms      = 0u;
                a->leds.pulse_idx = 0u;

                a->adc_currents.pcm1_lsb  = 0u;
                a->adc_currents.acm1_lsb  = 0u;
                a->adc_pos.enc1_lsb       = 0u;
                a->adc_stale_ms = 0u;

                a->i_pcm1_A = 0.0;
                a->i_acm1_A = 0.0;
                a->i_meas1_A_last = 0.0;
                a->i_ref1_A = 0.0f;

                a->pcm_offset_lsb = 0.0f;
                a->pcm_offset_v = PCM_VOFFSET_V;
                a->pcm_off_cal.acc = 0;
                a->pcm_off_cal.n = 0;
                a->pcm_off_cal.done = false;

                a->general_cnt = 0;

                a->state = APP_POWER_PRECHECK;

              break;

            case APP_POWER_PRECHECK:

                if (BoardPwr_IsPowerOk(&a->pwr)) {
                    // partiamo con calibrazione di POS
                    Uart_TrySendPos(20, a->adc_pos.enc1_lsb);
                    if (a->pcm_off_cal.done){
                        a->state = APP_TESTING;
                    }
                   }
            break;

            case APP_TESTING:

                ///////////////////////// GENERATORE DI RIFERIMENTI DI CORRENTE PER LOOP CORRENTE CHIUSO ///////////////////////////
                //NEW: Generatore riferimento corrente a 1 kHz (se attivo)
                IRef_Tick1kHz(&a->iref);

                if (a->iref.active) {
                    a->i_ref1_A = IRef_GetA(&a->iref);
                    // soft clamp di sicurezza contro configurazioni alterate
                    if (a->i_ref1_A > I_SOFT_MAX_A)  a->i_ref1_A = I_SOFT_MAX_A;
                    if (a->i_ref1_A < -I_SOFT_MAX_A) a->i_ref1_A = -I_SOFT_MAX_A;
                }

                if(LimitCurrent_TRIPM1(&a->ctrl)){
                    a->faults |= APPF_OVERCURRENT;
                }

                ///////////////////////// DEBUG ///////////////////////////////////////////
                int16_t iref_mA = (int16_t)lrintf(a->i_ref1_A  * 1000.0f);
                int16_t imeas_mA = (int16_t)lrintf(a->i_pcm1_A * 1000.0f);
                int16_t acm_meas_mA = (int16_t)lrintf(a->i_acm1_A * 1000.0f);
                int16_t e_mA = (int16_t)lrintf((a->i_ref1_A - a->i_pcm1_A) * 1000.0f);
                int16_t tensione_bridge = (int16_t)lrintf(a->ctrl.v_tripm1_V);

                uint16_t duty_perm = (uint16_t)lrintf(a->ctrl.duty * 100.0f);
                uint16_t correzione_duty = (uint16_t)lrintf(a->ctrl.correzione_duty * 1.0f);

                int16_t pos_x = a->adc_pos.enc1_lsb * (ADC_VREF_V / ADC_FS_LSB);
                int16_t pos_x_V = pos_x * ((R49+R50)/R49);

                /* ============== Telemetria su Python ============== */
                //Uart_TrySendCurSigned_mA(0/*acm_meas_mA*/, imeas_mA);
                //Uart_TrySendCur_mA(0, pos_x_V);
                //Uart_TrySendPos(duty_perm, 0/*a->adc_pos.enc1_lsb*/);

                /* ============== Telemetria su Qt GUI ============== */
                // ======= TX ========
                UartTelemetry_Tick(
                    /*Duty*/       correzione_duty, duty_perm,
                    /*Correnti*/   imeas_mA, iref_mA,
                    /*Voltaggi*/   0,  tensione_bridge,
                    /*Posizioni*/  0,  pos_x_V
                );

                // ======= RX =======
                UartRx_Poll(&a->s_rx, 64u);  // parser dei bytes

                UartTelemetryType sub;       // variabile di appoggio: ID del segnale che Qt sta tunando (es. Duty/Current/Position)
                int16_t minRaw, maxRaw;      // variabile di appoggio: parametri del comando SignalTuning nel formato fixed-point che arriva da Qt
                uint16_t freq;               // variabile di appoggio: frequenza in Hz definita in Qt

                while (UartRx_PopSignal(&a->s_rx, &sub, &minRaw, &maxRaw, &freq)) // consumiamo ciò che è arrivato da Qt ogni volta che il parser valida un frame impostando pending = true
                {
                    TuningStore_ApplySignal(&a->tune, (uint8_t)sub, minRaw, maxRaw, freq); // aggiorna i parametri del segnale appena poppato

                    // Decodifica dei parametri nel formato fixed-point e applicali alle variabili operative
                    switch (sub)
                    {
                        case UART_TLM_DUTY:{

                            //float dmin = UartDecode_Duty01(&a->tune.duty.min_raw);
                            a->ctrl.duty_gain = UartDecode_Duty01(&a->tune.duty.max_raw);
                            a->ctrl.duty_freq = a->tune.duty.freq_hz;

                        }break;

                        case UART_TLM_CURRENT:

                            a->iref.step_A = UartDecode_CurrentA(&a->tune.current.max_raw);

                        break;

                        case UART_TLM_POSITION:

                        break;

                    }
                }

                ///////////////////////////////////////////////////////////////////////////////////

            break;

            case APP_READY:

                  // Idle controllato: coast finché non decidi di attivare (oppure passa diretto ad ACTIVE)
                  //go_safe_coast();

                  // qui potresti entrare in ACTIVE in base a un trigger (es. comando o timer)
                  a->state = APP_CONTROL_ACTIVE;
                  SpGen_Arm(&a->spg, 2048);
                  // Chiudi il canale NC_rly (sicurezza)
                  Laser_Enable(true);
                  a->pwr.laser_ok = true;

              break;

            case APP_CONTROL_ACTIVE:

                  if (!got_pos) break;      // aspetta sempre il sample ADC pulito

                  // Controllo: PID + policy attuazione
                  {
                      uint16_t target = SpGen_Get(&a->spg);   // oppure valore proveniente da host
                      uint16_t y_pos  = a->adc_pos.enc1_lsb;  // posizione JSL asse X

                    // ===== Telemetria =====

                    //Uart_TrySendPos(target, y_pos);                        // ENC1 a 50 Hz
                    //Uart_TrySendCur(a->adc_set.acm_lsb, a->adc_set.pcm_lsb);           // CORRENTI a 50 Hz (LSB raw)
                    //Uart_TrySendCur_mA(0, (uint16_t)lrintf(a->i_pcm1_A * 1000.0f));

                    // Anello esterno: PD posizione -> I_ref_A (corrente richiesta)
                    PlantControl_Step1kHz(&a->ctrl, target, y_pos);


                    Uart_TrySendCur(0, (uint16_t)lrintf(a->i_pcm1_A * 1000.0f));
                    //Uart_TrySendCur_mA(0, &a->cur_pi.e_A);
                  }

              break;

            case APP_FAULT:

                  // Safe state hard: coast e VMOTEN resterà basso grazie a BoardPwr

                // Apri il canale NC_rly
                Laser_Enable(false);
                a->pwr.laser_ok = false;

                go_safe_coast();
                  // Qui puoi decidere se rientrare automaticamente quando i fault si azzerano
                  // oppure aspettare un reset/clear manuale.
              break;

            case APP_OVERCURRENT:   // PER USCIRE DALLA CONDIZIONE DI FAULT DATA DA SOVRACORRENTE BISOGNA IMPOSTARE UNA ISTERESI TEMPORALE DURANTE LA QUALE IL SISTEMA STA FERMO (es.5s). DOPO QUESTO TEMPO ANDIAMO A VEDERE SE LA TENSIONE v_mot1p_V RISALE SOPRA I 9V (COSA CHE DOVREBBE SUCCEDERE PER LA LOGICA DEI FUSIBILI TERMICI)
                BoardPwr_EnableVmot(false);
                if(++a->general_cnt >= 5000){
                    BoardPwr_EnableVmot(true);
                    a->state = APP_TESTING;
                }
                //Uart_TrySendCurSigned_mA(0, (int16_t)lrintf(a->ctrl.v_tripm1_V * 1000.0f));
              break;

            case APP_SHUTDOWN:
            default:

                Btn_TriggerHarakiri(&a->btn);                         // Harakiri sparato sul pin HW

              break;
      }
}

void AppOrch_Tick10kHz(AppOrch* a)
{
    AdcCurrents cur;
    if (!DrvAdc_ReadCurr_10kHz(&cur)) return;

    a->adc_currents = cur;                                      // rende il burst a 10kHz disponibile a chiunque nel resto dell’app

    /* === Corrente approx (IPROPI -> ACM, senza segno) === */
    a->i_acm1_A = acmLSB_to_A(cur.acm1_lsb);

    /* === Calcolo del offset PCM (solo in fase PRECHECK o APP_BOOT) === */
    if (!a->pcm_off_cal.done && (a->state == APP_POWER_PRECHECK || a->state == APP_BOOT)) {
        a->pcm_off_cal.acc += (uint32_t)cur.pcm1_lsb;
        a->pcm_off_cal.n++;
        if (a->pcm_off_cal.n >= PCM_OFFSET_NSAMPLES) {
            a->pcm_offset_lsb = (float)a->pcm_off_cal.acc / (float)a->pcm_off_cal.n;
            a->pcm_offset_v   = a->pcm_offset_lsb * (ADC_VREF_V / (float)ADC_FS_LSB);
            a->pcm_off_cal.done = true;
            a->pcm_off_cal.acc = 0;
            a->pcm_off_cal.n = 0;
        }
    }

    /* === PCM: corrente precisa (bipolare) === */
    a->i_pcm1_A = pcmLSB_to_A_IIR(cur.pcm1_lsb, a->pcm_offset_v, a->pcm_alpha, &a->pcm_iir);
    //a->i_pcm1_A = pcmLSB_to_A(cur.pcm1_lsb);

    /* === Fusibile termico F2 (canale TRIPM1): monitora sovracorrente === */
    a->ctrl.v_tripm1_V = a->adc_currents.trip1_lsb * (ADC_VREF_V / ADC_FS_LSB);    // tensione ai capi del fusibile termico nel range del ADC [0,3.3V]
    a->ctrl.v_tripm1_V = a->ctrl.v_tripm1_V * ((R35+R33)/R35);                      // tensione ai capi del fusibile termico nel range [0, 12V]. Questo valore deve essere al minimo 9V. Se scende sotto 9V bisogna disabilitare il ponte H perchè significa che sono in sovracorrente
    a->ctrl.v_tripm1_V = clamp(a->ctrl.v_tripm1_V, 0.0f, 12.0f);

    /* === Modalità test: genera stimoli e attua open-loop === */
      if (a->state == APP_TESTING){

          /* LOOP CORRENTE ATTIVO con i_ref triangolare */
          if (a->iref.active && a->pcm_off_cal.done && BoardPwr_IsPowerOk(&a->pwr)) {

              /* ESECUZIONE PI CORRENTE: Prendi il riferimento aggiornato a 1 kHz */
            /*  float u_cmd_V = CurPi_Step(&a->cur_pi, a->i_ref1_A, a->i_pcm1_A); // [V]

              // Traduci questa tensione u_cmd_V in comando in duty/dir
             float Vmag = fabsf(u_cmd_V);
             if (Vmag < 1e-3f) {
                 DrvPwm_Coast();
                 return;
             }

             a->ctrl.cfg.duty = Vmag / a->ctrl.cfg.Vmot_V;
             if (a->ctrl.cfg.duty > 1.0f) a->ctrl.cfg.duty = 1.0f;

             ActDir dir = (u_cmd_V >= 0.0f) ? a->ctrl.cfg.dir_to_increase
                                          : invert_dir(a->ctrl.cfg.dir_to_increase);    */

              // CONTROLLO CORRENTE A LOOP APERTO
              //PlantSetDutyDir(&a->ctrl);          // duty cycle onda quadra
              PlantSetDutyDir_Sine10Hz(&a->ctrl);   // duty cycle onda sinusoidale

              a->ctrl.correzione_duty = a->ctrl.cfg.Vmot_V / a->ctrl.v_tripm1_V;
              a->ctrl.correzione_duty = clamp(a->ctrl.correzione_duty, 0, 1);   // clamp di sicurezza sul fattore di correzione del dutycycle

              a->ctrl.duty = a->ctrl.duty * a->ctrl.correzione_duty;
              a->ctrl.duty = clamp(a->ctrl.duty, 0, 1); // clamp di sicurezza sul dutycycle del segnale PWM

              DrvPwm_Drive(a->ctrl.direzione, a->ctrl.duty);

          }

      } else {
          /* ESECUZIONE PI CORRENTE: Prendi il riferimento aggiornato a 1 kHz */
                        float u_cmd_V = CurPi_Step(&a->cur_pi, a->i_ref1_A, a->i_pcm1_A); // [V]

                        // Traduci questa tensione u_cmd_V in comando in duty/dir
                       float Vmag = fabsf(u_cmd_V);
                       if (Vmag < 1e-3f) {
                           DrvPwm_Coast();
                           return;
                       }

                       a->ctrl.duty = Vmag / a->ctrl.cfg.Vmot_V;
                       if (a->ctrl.duty > 1.0f) a->ctrl.duty = 1.0f;

                       ActDir dir = (u_cmd_V >= 0.0f) ? a->ctrl.cfg.dir_to_increase
                                                    : invert_dir(a->ctrl.cfg.dir_to_increase);

                       DrvPwm_Drive(dir, a->ctrl.duty);
      }
}

AppState AppOrch_State(const AppOrch* a){
    return a->state;
}
uint32_t AppOrch_Faults(const AppOrch* a){
    return a->faults;
}
void AppOrch_RequestShutdown(AppOrch* a){
    a->faults |= APPF_USER_SHUTDOWN;
}
