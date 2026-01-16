/*
 * drv_rti.c
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 */


#include <control_timer/drv_rti.h>

/* Stato interno (modulo)
 * static:      visibile solo a questo file (scope file-local)
 * volatile:    varia in contesto ISR
 */
static volatile uint8_t  s_tick_1k       = 0;       // 1 se ISR ha segnalato un nuovo tick
static volatile uint32_t s_count_tick_1k = 0;       // millisecondi da boot
static DrvRti_TickHook   s_hook_1k       = NULL;    // puntatore alla funzione registrata (opzionale)


//
static volatile uint8_t  s_tick_10k       = 0;
static volatile uint32_t s_count_tick_10k = 0;
static DrvRti_TickHook   s_hook_10k       = NULL;

void DrvRti_Init_1kHz(void)
{

    ////////////////////// RTI @1ms //////////////////////
    rtiInit();
    rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE0);        // Enable RTI Compare 0 interrupt notification
    _enable_IRQ_interrupt_();                                       // Enable IRQ - Clear Interrupt flag in CPS register
    rtiStartCounter(rtiREG1,rtiCOUNTER_BLOCK0);                     // Start RTI Counter Block 0*/

}

void DrvRtiFast_Init_10kHz(void)
{
    ////////////////////// RTI @100 µs //////////////////////
    /* Presuppone rtiInit() già chiamata altrove; in caso contrario richiamalo qui. */

    rtiEnableNotification(rtiREG1, rtiNOTIFICATION_COMPARE1);
    _enable_IRQ_interrupt_();
    rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);
}

bool DrvRti_ConsumeTick(void)
{
    if (s_tick_1k) {
        s_tick_1k = 0;  // consume
        if (s_hook_1k) {     // se c'è un hook, lo invoca
            s_hook_1k();
        }
        return true;
    }
    return false;
}

bool DrvRtiFast_ConsumeTick(void)
{
    if (s_tick_10k) {
        s_tick_10k = 0;
        if (s_hook_10k) {
            s_hook_10k();
        }   // opzionale: gestisci qui l’hook fast
        return true;
    }
    return false;
}

uint32_t DrvRti_Millis(void)
{
    return s_count_tick_1k;
}

void DrvRti_RegisterHook(DrvRti_TickHook hook)
{
    /* 'hook è pensato per essere molto leggero
     * e viene eseguito nel contesto del main, NON in ISR.
    */
    s_hook_1k = hook; // Registra la funzione hook
}

void DrvRti_RegisterHook_Fast(DrvRti_TickHook hook)
{
    s_hook_10k = hook;
}

/* ========================= ISR  =========================
 * Questa funzione è il weak symbol chiamato dal vettore IRQ per RTI Compare0.
 * Mantenerla leggerissima: niente float, niente I/O, nessuna stampa.
 */
void rtiNotification(rtiBASE_t *rti, uint32 notification)
{
    (void)rti;

    if (notification == rtiNOTIFICATION_COMPARE0) {
        s_tick_1k = 1;
        s_count_tick_1k++;
        return;
    }

    if (notification == rtiNOTIFICATION_COMPARE1) {
        s_tick_10k = 1;
        s_count_tick_10k++;
        return;
    }

    //Possible alternativa dato che notification è una bitmask
    /*if (notification & rtiNOTIFICATION_COMPARE0){ s_tick_1k = 1; s_count_tick_1k++; }
  if (notification & rtiNOTIFICATION_COMPARE1){ s_tick_10k = 1; s_count_tick_10k++; }*/

}
