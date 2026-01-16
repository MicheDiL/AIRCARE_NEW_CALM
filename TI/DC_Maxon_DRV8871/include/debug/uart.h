/*
 * uart.h
 *
 *  Created on: 3 nov 2025
 *      Author: mdilucchio
 *
 *      Questo modulo serve per preparare i pacchetti da 8byte e accodarli nel ring buffer.
 *      Modulo utilizzato pre streaming UART e lettura con python.
 */

#ifndef INCLUDE_DEBUG_UART_H_
#define INCLUDE_DEBUG_UART_H_

/*************************************************** Include Files **************************************************/
#include "debug/drv_sci.h"
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/
/* Header distinti per tipo telemetria */
#define TLM_HDR_POS   0xA55Au   // target & enc1
#define TLM_HDR_CUR   0xC0A1u   // acm & pcm
#define TLM_HDR2_CUR  0xC0A2u   // i_ref_mA_s16(bias), i_meas_mA_s16(bias)


/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef struct __attribute__((packed)) { // definiamo la struttura del pacchetto da mandare via Uart. Il pacchetto è formato da 8byte
    uint16_t hdr;       // magic
    uint16_t a;         // payload unsigned
    uint16_t b;         // payload unsigned
    uint16_t crc;       // XOR semplice
} TelemetryPkt;
/********************************************************************************************************************/

/************************************************ Function declarations *********************************************/
void Uart_Init(uint16_t period_pos_ms, uint16_t period_cur_ms);         // es. 20 ms = 50 Hz
void Uart_TrySendPos(uint16_t target_lsb, uint16_t enc1_lsb);
void Uart_TrySendCur(uint16_t acm_lsb, uint16_t pcm_lsb);               // Unità on-wire: LSB ADC grezzi (0..4095) => la conversione in A la fa il consumer (PC/logger)
void Uart_TrySendCur_mA(uint16_t acm_mA_u16, int16_t pcm_mA_s16);       // Unità on-wire: mA già scalati a bordo   => il consumer riceve numeri “umani” subito grafabili; no formule lato PC

/* Telemetria corrente: i_ref e i_meas entrambi signed in mA (offset-binary) */
void Uart_TrySendCurSigned_mA(int16_t iref_mA, int16_t imeas_mA);

/********************************************************************************************************************/

#endif /* INCLUDE_DEBUG_UART_H_ */
