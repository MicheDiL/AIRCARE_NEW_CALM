/*
 * signaling_leds.h
 *
 *  Created on: 13 nov 2025
 *      Author: mdilucchio
 */

#ifndef INCLUDE_LEDS_SIGNALING_LEDS_H_
#define INCLUDE_LEDS_SIGNALING_LEDS_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "HL_gio.h"
#include "HL_mibspi.h"   // I simboli delle porte mibspiPORTx arrivano da HL_mibspi.h
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
#define LED_RED_PORT        mibspiPORT5
#define LED_RED_PIN         PIN_CS0

#define LED_GRN_PORT        mibspiPORT4
#define LED_GRN_PIN         PIN_SIMO

#define LED_BLU_PORT        mibspiPORT4
#define LED_BLU_PIN         PIN_CS3

#define LED_RED_ACTIVE_HIGH true
#define LED_GRN_ACTIVE_HIGH true
#define LED_BLU_ACTIVE_HIGH true
/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef struct {
    gioPORT_t* port;                // es. mibspiPORT4, mibspiPORT5
    uint32_t   pin;                 // es. PIN_CS0, PIN_SIMO, PIN_CS3 (come configurati in HAL)
    bool       active_high;         // true: livello alto = LED ON; false: livello basso = LED ON
} Led;

// Dichiaro tre istante/oggetti di Led che sono definite da qualche altra parte (extern)
extern const Led LED_RED;
extern const Led LED_GRN;
extern const Led LED_BLU;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void Led_Init(void);                    // idempotente; porta tutti i LED a OFF
void Led_Set (const Led* l, bool on);   // ON/OFF logico
void Led_On  (const Led* l);
void Led_Off (const Led* l);
void Led_Toggle(const Led* l);
bool Led_IsOn(const Led*);         // stato logico (indipendente dalla polarità)
/********************************************************************************************************************/

#endif /* INCLUDE_LEDS_SIGNALING_LEDS_H_ */
