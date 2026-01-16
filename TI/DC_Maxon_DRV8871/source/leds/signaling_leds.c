/*
 * signaling_leds.c
 *
 *  Created on: 13 nov 2025
 *      Author: mdilucchio
 */

#include "leds/signaling_leds.h"

/* macro per la traduzione logico->fisico*/
#define _PHYS(on, ah)  ((uint32_t)((ah) ? ((on)?1u:0u) : ((on)?0u:1u))) // traduce il comando logico on (true/false) nel livello fisico da scrivere a GPIO (quindi 0 oppure 1), tenendo conto della polarità (ah = active_high).

/* definizioni degli oggetti dichiarati extern nell’header */
const Led LED_RED = { LED_RED_PORT, LED_RED_PIN, LED_RED_ACTIVE_HIGH };
const Led LED_GRN = { LED_GRN_PORT, LED_GRN_PIN, LED_GRN_ACTIVE_HIGH };
const Led LED_BLU = { LED_BLU_PORT, LED_BLU_PIN, LED_BLU_ACTIVE_HIGH };

/* micro-funzioni Helper => usiamo inline per evitare overhead di chiamata*/
static inline uint32_t _read_phys(const Led* l) {
    return (uint32_t)gioGetBit(l->port, l->pin);
}
static inline void _write_phys(const Led* l, uint32_t v) {
    gioSetBit(l->port, l->pin, v & 1u);     // & 1u garantisce che passi solo il bit LSB, evitando valori “sporchi”
}

void Led_Init(void)
{
    gioInit();
    /* Init mibspi to initialize led pins as gpio */
    mibspiInit();

    /* Portiamo tutti i LED a OFF. */
    Led_Off(&LED_RED);
    Led_Off(&LED_GRN);
    Led_Off(&LED_BLU);
}

void Led_Set(const Led* l, bool on)
{
    _write_phys(l, _PHYS(on, l->active_high));

    // on=true,     active_high=true -> scrive 1 (LED ON)
    // on=false,    active_high=true -> scrive 0 (LED OFF)
    // on=true,     active_high=false -> scrive 0 (LED ON perché è attivo-basso)
    // on=false,    active_high=false -> scrive 1 (LED OFF)

}

void Led_On(const Led* l)  { Led_Set(l, true);  }
void Led_Off(const Led* l) { Led_Set(l, false); }

void Led_Toggle(const Led* l)
{
    /* Toggle logico, indipendente dall'active_high */
    const uint32_t phys = _read_phys(l);
    const bool is_on = l->active_high ? (phys!=0u) : (phys==0u);
    Led_Set(l, !is_on);
}

bool Led_IsOn(const Led* l)
{
    const uint32_t phys = _read_phys(l);
    return l->active_high ? (phys!=0u) : (phys==0u);
}
