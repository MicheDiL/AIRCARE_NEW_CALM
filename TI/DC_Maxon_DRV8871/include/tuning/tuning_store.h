/*
 * tuning_store.h
 *
 *  Created on: 15 gen 2026
 *      Author: mdilucchio
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
/* Cosa arriva da Qt: min/max raw + freq, e PID in milli */
typedef struct {
    int16_t  min_raw;
    int16_t  max_raw;
    uint16_t freq_hz;
    bool     valid;
} TuneSignal;

typedef struct {
    int32_t kp_milli;
    int32_t ki_milli;
    int32_t kd_milli;
    bool    valid;
} TunePid;

typedef struct {
    /* segnali */
    TuneSignal duty;
    TuneSignal current;
    TuneSignal position;

    /* pid */
    TunePid    pid0;    // se per ora hai un solo PID, basta questo

    /* monotonic counter utile per capire se qualcosa è cambiato */
    uint32_t   version;
} TuningStore;

/************************************************ Function declarations *********************************************/
void TuningStore_Init(TuningStore* s);

/* Apply “raw” da parser */
void TuningStore_ApplySignal(TuningStore* s, uint8_t sub, int16_t minRaw, int16_t maxRaw, uint16_t freq_hz);
void TuningStore_ApplyPid   (TuningStore* s, uint8_t pidId, int32_t kp_m, int32_t ki_m, int32_t kd_m);

/* Getter (letture veloci) */
uint32_t TuningStore_Version(const TuningStore* s);
/********************************************************************************************************************/

#endif /* INCLUDE_TUNING_TUNING_STORE_H_ */
