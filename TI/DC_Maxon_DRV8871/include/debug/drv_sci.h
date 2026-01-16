/*
 * drv_sci.h
 *
 *  Created on: 4 nov 2025
 *      Author: mdilucchio
 *
 *      Questo modulo è delegato all'invio fisico dei pacchetti senza busy-wait.
 *      Strategia: coda circolare (ring) in RAM + trasmissione a blocchi con sciSend()
 *      in modalità interrupt. La UART svuota i blocchi tramite la sua ISR; il ciclo a 1 kHz non resta mai appeso
 *      Tutta la trasmissione fisica a baud rate viene pompata in interrupt (IRQ mode), fuori dal ciclo di controllo da 1 kHz.
 *
 *
 *      13/01/26
 *      Abbiamo aggiunto anche la ricezione ad interrupt.
 *      drv_sci diventa un transport layer full-duplex:
 *      TX ring (già ce l’hai) + IRQ TX
 *      RX ring + IRQ RX (nuovo): RX ISR fa solo enqueue in un ring buffer RX
 *      Il tuo loop 1 kHz (o task di basso livello) chiama UartCmd_Tick(...) che:
 *          - legge dal ring RX con un budget (es. max 64 byte/tick),
 *          - fa parsing robusto (resync su kMagic),
 *          - valida CRC,
 *          - chiama callback/apply (dove tu aggiorni i parametri di tuning del controllo)
 */

#ifndef INCLUDE_DEBUG_DRV_SCI_H_
#define INCLUDE_DEBUG_DRV_SCI_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "HL_sci.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
#ifndef SCI_TX_RING_SIZE
#define SCI_TX_RING_SIZE 512u
#endif

#ifndef SCI_RX_RING_SIZE
#define SCI_RX_RING_SIZE 512u
#endif

#define sciREGx sciREG3 //sciREG4 se usiamo pcb CALM, sciREG3 se usiamo il launchpad
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/

/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
// API TX
void     DrvSci_Init(void);                                 // chiama sciInit(), abilita IRQ TX
bool     DrvSci_Write(const uint8_t* data, uint16_t len);   // enqueue non bloccante (drop se pieno)
uint32_t DrvSci_Dropped(void);                              // contatore drop (debug)

// API RX
uint16_t DrvSci_RxAvailable(void);                         // ritorna quanti byte sono attualmente nel ring RX (pronti da consumare nel main)
bool     DrvSci_ReadByte(uint8_t* out);                    // legge 1 byte (false se vuoto)
uint16_t DrvSci_Read(uint8_t* buf, uint16_t maxlen);       // legge fino a maxlen byte (non blocca). Ritorna quanti ne ha letti
uint32_t DrvSci_RxDropped(void);                           // drop counter RX (byte persi se ring pieno:se cresce, significa che il budget di parsing è insufficiente o il ring è troppo piccolo)
/********************************************************************************************************************/

#endif /* INCLUDE_DEBUG_DRV_SCI_H_ */
