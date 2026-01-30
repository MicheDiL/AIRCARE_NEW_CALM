/*
 * waveform_engine.h
 *
 *  Created on: 29 gen 2026
 *      Author: mdilucchio
 *
 *      Serve a trasformare i parametri di tuning ricevuti via UART (in fixed-point raw) in un set
 *      di generatori di forme d’onda che, nel loop a 10 kHz, producono un valore “fisico” (float)
 *      coerente con le scale Qt-RM57
 */

#ifndef INCLUDE_TARGET_GENERATORS_WAVEFORM_ENGINE_H_
#define INCLUDE_TARGET_GENERATORS_WAVEFORM_ENGINE_H_

#include <stdint.h>
#include <stdbool.h>
#include "tuning/tuning_store.h"   // TuneWaveform, TuneSignalShape, TuningStore

/* Tipi di forme d'onda eseguibili dal generatore di un singolo segnale */
typedef enum {
    WGEN_SIN            = 0u,
    WGEN_STEPTRIANGLE   = 1u,
    WGEN_CONST          = 2u
} WaveGenShape;

/* Tipo del segnale (serve per decode raw->fisico coerente con Qt) */
typedef enum {
    WGEN_T_DUTY = 0,     // % *100
    WGEN_T_CURRENT = 1,  // mA *10
    WGEN_T_VOLTAGE = 2,  // V *1000 (non usato nei generatori)
    WGEN_T_POSITION = 3  // deg *1000
} WaveGenSignalType;

/* Configurazione in unità fisiche (float), pronta per essere usata dal generatore */
typedef struct {
    WaveGenShape shape;

    float min_v;
    float max_v;

    /* sin */
    float freq_hz;     // Hz

    /* step triangle */
    float step_amp;    // stessa unità del segnale (%, mA, deg)
    uint16_t step_ms;  // ms
} WaveGenCfg;

/* Stato interno generatore di un singolo segnale */
typedef struct {
    WaveGenSignalType type; // (Duty/Current/Position) -> serve per sapere come decodificare i raw
    float fs;               // sample rate (es. 10000 Hz) a cui avviene la generazione del target
    float dt;               // passo temporale 1/fs

    WaveGenCfg cfg_active; // parametri di configurazione realmente usati dal generatore di target nel loop a 10kHz
    WaveGenCfg cfg_next;   // nuovi parametri di configurazione ottenuti dal tuning Qt ma non ancora usati dal generatore di target
    bool has_next;         // flag che dice che c'è stato un update dei parametri di configurazione per il generatore di target

    /* stato runtime */
    float y;               // output corrente

    // Sinusoide
    float phase;           // per sin
    float omega;           // 2*pi*f

    // Triangolare a gradini
    int dir;               // per step triangle (+1/-1)
    uint32_t step_ticks;   // quanti sample tra uno step e il prossimo
    uint32_t step_cnt;     // contatore

} WaveGen;

/* Contenitore dei generatori per i 3 segnali tunabili */
typedef struct {
    WaveGen duty;
    WaveGen current;
    WaveGen position;
    uint32_t last_store_version;
} SignalGens;

/* API */
void WaveGen_Init(WaveGen* g, WaveGenSignalType type, float sample_rate_hz);
void WaveGen_StageFromTune(WaveGen* g, const TuneWaveform* w);  // w in raw, decodifica e prepara cfg_next
void WaveGen_CommitIfStaged(WaveGen* g);                        // commit atomico “logico”

float WaveGen_Step(WaveGen* g);                                 // step 1 sample (10kHz)

void SignalGens_Init(SignalGens* sg, float sample_rate_hz);
void SignalGens_UpdateFromStore(SignalGens* sg, const TuningStore* s); // Sceglie quale generatore aggiornare

#endif /* INCLUDE_TARGET_GENERATORS_WAVEFORM_ENGINE_H_ */
