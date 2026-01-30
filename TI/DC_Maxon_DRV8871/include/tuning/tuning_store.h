/*
 * tuning_store.h
 *
 *  Created on: 15 gen 2026
 *      Author: mdilucchio
 *
 *      Consumer utilizzato dal ciclo di controllo per svuotare i pending di UartRxCtx e trasformarli in uno stato consistente
 *      "ultimo valore valido" che il controllo può leggere quando vuole.
 */

#ifndef INCLUDE_TUNING_TUNING_STORE_H_
#define INCLUDE_TUNING_TUNING_STORE_H_

/*************************************************** Include Files **************************************************/
#include <stdint.h>
#include <stdbool.h>
/********************************************************************************************************************/

/*************************************************** Define macros **************************************************/

/********************************************************************************************************************/

/************************************************** Type definitions ************************************************/
typedef enum {
    TUNE_SHAPE_SIN              = 0u,
    TUNE_SHAPE_STEPTRIANGLE     = 1u,
    TUNE_SHAPE_CONST            = 2u
} TuneSignalShape;

typedef struct {
    uint8_t  id;
    uint8_t  shape;

    int16_t  min_raw;
    int16_t  max_raw;

    int16_t  aux1_raw;
    uint16_t aux2;

    bool valid;         // flag che ci dice se le modifiche di tuning sono andate a buon fine
}TuneWaveform;

typedef struct {
    int32_t kp_milli;
    int32_t ki_milli;
    int32_t kd_milli;
    bool    valid;
} TunePid;

typedef struct {
    /* segnali */
    TuneWaveform duty;
    TuneWaveform current;
    TuneWaveform position;

    /* pid */
    TunePid    pid0;    // se per ora hai un solo PID, basta questo

    /* counter utile al controllo per capire se qualcosa è cambiato cioè qualche parametro è mai stato tunato */
    uint32_t   version;
} TuningStore;

/************************************************ Function declarations *********************************************/
void TuningStore_Init(TuningStore* s);

/* Apply “raw” da parser */

void TuningStore_ApplyWaveform(TuningStore* s,
                               uint8_t sub,
                               uint8_t id,
                               uint8_t shape,
                               int16_t minRaw,
                               int16_t maxRaw,
                               int16_t aux1Raw,
                               uint16_t aux2);


void TuningStore_ApplyPid   (TuningStore* s, uint8_t pidId, int32_t kp_m, int32_t ki_m, int32_t kd_m);

/* Getter (letture veloci) */
uint32_t TuningStore_Version(const TuningStore* s);
/********************************************************************************************************************/

#endif /* INCLUDE_TUNING_TUNING_STORE_H_ */
