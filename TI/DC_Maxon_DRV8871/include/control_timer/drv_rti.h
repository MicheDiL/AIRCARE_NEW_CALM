/*
 * drv_rti.h
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 *
     * Driver RTI bare-metal (1 kHz) per RM57 + HALCoGen.
     * - ISR: alza un flag e incrementa millisecondi.
     * - API: DrvRti_ConsumeTick() da chiamare nel while(1) (cooperativo).
     * - Nessuna logica applicativa in ISR.

 */

#ifndef INCLUDE_CONTROL_TIMER_DRV_RTI_H_
#define INCLUDE_CONTROL_TIMER_DRV_RTI_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "HL_rti.h"
#include "HL_sys_core.h"   // _enable_IRQ_interrupt_
#include "HL_sys_vim.h"    // se usi VIM API (opzionale)
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
/* Hook = puntatore a funzione che permette all'applicazione di “inserire” del codice personalizzato in punti
 * prestabiliti del modulo senza modificare il modulo stesso. È una forma semplice di inversione del controllo (callback).*/
typedef void (*DrvRti_TickHook)(void); // definisco il tipo puntatore a funzione HOOK (prototipo void fn(void))

/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
// Init: inizializza RTI e abilita IRQ
void DrvRti_Init_1kHz(void);

// API cooperativa che il main chiama per sapere se è arrivato un nuovo ms (ritorna true se sì e “consuma” il tick)
bool DrvRti_ConsumeTick(void);

// Millisecondi da avvio
uint32_t DrvRti_Millis(void);

// permette di registrare una callback “light” (HOOK) che viene eseguita quando si consuma un tick
void DrvRti_RegisterHook(DrvRti_TickHook hook);
void DrvRti_RegisterHook_Fast(DrvRti_TickHook hook);

//
void DrvRtiFast_Init_10kHz(void);
bool DrvRtiFast_ConsumeTick(void);
/********************************************************************************************************************/

#endif /* INCLUDE_CONTROL_TIMER_DRV_RTI_H_ */
