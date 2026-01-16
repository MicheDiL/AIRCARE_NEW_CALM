/*
 * board_power.h
 *
 *  Created on: 12 nov 2025
 *      Author: mdilucchio
 *
 *      Modulo che fa i check su nUPS_ALERT e nIPREMOVED prima di dare VMOTEN al ponte
 *      motore, abilitare l'uscita del laser — e che continua a monitorare i segnali a runtime
 *
 *      - nIPREMOVED (active-low) ti dice se l’ingresso 12 V è presente
 *      - nUPS_ALERT (active-low) va a 0 quando la riserva nei supercap non è più sufficiente a sostenere l’uscita: è il “tembo d’allarme” per il fermo controllato.
 *      - UPS_CPG    (active-high) è un’uscita open-drain che diventa “1” solo quando il banco supercap è ~>92% della tensione target,
 *                   cioè è l’uscita che indica che la batteria di supercap è carica e “buona” rispetto a una soglia programmata (ovvero ~>92% della tensione target)
 *                   e questa batteria fungerà da riserva energetica quando togliamo l'alimentazione alla scheda (ATTENZIONE: MISURANDO IL TP41 IL SUO LIVELLO NON SALE MAI E RIMANE FISSO A 0V!! PERCHE?)
 *
 *      obiettivo: abilitare VMOT solo quando la “power chain” è sana (ovvero quando l’ingresso è presente e non c’è alert) e, a runtime, togliere VMOT
 *                  in caso di degrado, senza busy-wait e senza bloccare il loop a 1 kHz
 */

#ifndef INCLUDE_POWER_UP_UPS_BOARD_POWER_H_
#define INCLUDE_POWER_UP_UPS_BOARD_POWER_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "HL_gio.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
/* === Mappa pin (sheet UPS) === */
#define BPWR_PORT_UPS_ALERT    gioPORTB  // nUPS_ALERT  active-low -> 0 = condizione d’allarme (energia residua, fault, ecc.); 1 = nessun alert
#define BPWR_BIT_UPS_ALERT     0u

#define BPWR_PORT_IP_REMOVED   gioPORTB  // nIPREMOVED  active-low -> 0 = ingresso 12 V mancante/instabile; 1 = ingresso presente
#define BPWR_BIT_IP_REMOVED    1u

#define BPWR_PORT_CAP_GOOD     gioPORTB  // UPS_CPG    active-high -> 1 = CAPGD: banco supercap “cap-good” (tensione > soglia con isteresi). 0 finché i cap non sono quasi pieni o se manca la pull-up sul pin MCU
#define BPWR_BIT_CAP_GOOD      2u

#define BPWR_PORT_VMOTEN       gioPORTA  // VMOTEN     active-high -> 1 abilita VMOTSW
#define BPWR_BIT_VMOTEN        5u

#define NCR_PORT_LASEREN       gioPORTA  //LASEREN     active-high
#define NCR_BIT_LASEREN        6u

/* Debounce (ms @1kHz) */
#ifndef BPWR_OK_DEBOUNCE_MS
#define BPWR_OK_DEBOUNCE_MS    50u      // quanto deve restare “OK” la condizione prima di abilitarci
#endif

#ifndef BPWR_BAD_DEBOUNCE_MS
#define BPWR_BAD_DEBOUNCE_MS   5u       // quanto deve restare “KO” prima di disabilitare
#endif
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef struct {
    bool input_present;         // flag che vogliamo che sia true quando l’ingresso 12 V è presente/stabile cioè quando nIPREMOVED = 1
    bool ups_alert;             // flag che vogliamo che sia true quando c’è un alert cioè quando nUPS_ALERT = 0
    bool cap_good;              // true se il banco supercap è OK                 (cap bank ok)
    bool vmot_enabled;          // true se VMOTEN alto
    bool laser_enabled;
} BoardPwrStatus;

typedef struct {
    BoardPwrStatus st;          // ultimo snapshot di BoardPwrStatus letto
    bool   power_ok;            // risultato del debounce: “siamo autorizzati ad avere VMOT attivo”
    bool   laser_ok;
    bool   fault_latched;       // diventa true alla prima transizione in KO (resta tale finché non chiami ClearFault)
    uint16_t ok_cnt, bad_cnt;   // contatori di stabilità
} BoardPwrCtx;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void BoardPwr_Init(BoardPwrCtx* c);

/* Enable/Disable VMOT esplicito (se serve manualmente) */
void BoardPwr_EnableVmot(bool on);

void Laser_Enable(bool on);

/* Snapshot immediato (senza debounce) */
BoardPwrStatus BoardPwr_ReadStatus(void);

/* Da chiamare ogni 1 ms */
void BoardPwr_Tick1kHz(BoardPwrCtx* c);

void BoardPwr_ClearFault(BoardPwrCtx* c);   /* opzionale: sblocca il latch */


/* Utility/Getter */
bool BoardPwr_IsPowerOk(const BoardPwrCtx* c);
bool BoardPwr_IsFaultLatched(const BoardPwrCtx* c);

/********************************************************************************************************************/

#endif /* INCLUDE_POWER_UP_UPS_BOARD_POWER_H_ */
