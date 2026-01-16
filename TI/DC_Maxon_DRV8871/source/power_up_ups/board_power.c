/*
 * board_power.c
 *
 *  Created on: 12 nov 2025
 *      Author: mdilucchio
 */

#include "power_up_ups/board_power.h"
#include "leds/signaling_leds.h"

////////////////////////////////// LEGGI LO STATO DEI PIN  ////////////////////////////////////////

static inline bool _bit(const gioPORT_t* p, uint32_t b){
    return (gioGetBit((gioPORT_t*)p, (uint32_t)b) != 0u);
}

/* Active-low -> true quando la linea è a 0 */
static inline bool _ups_alert_asserted(void){
    return !_bit(BPWR_PORT_UPS_ALERT, BPWR_BIT_UPS_ALERT);
}
static inline bool _ip_removed_asserted(void){
    return !_bit(BPWR_PORT_IP_REMOVED, BPWR_BIT_IP_REMOVED);
}
/* Active-high -> true se UPS_CPG == 1 */
static inline bool _cap_good(void){
    return  _bit(BPWR_PORT_CAP_GOOD, BPWR_BIT_CAP_GOOD);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/* Legge lo stato attuale del pin VMOTEN */
static inline bool _vmoten_read(void){
    return  _bit(BPWR_PORT_VMOTEN, BPWR_BIT_VMOTEN);
}
/* Forza il pin alto/basso */
static inline void _vmoten_write(bool on){
    gioSetBit(BPWR_PORT_VMOTEN, BPWR_BIT_VMOTEN, on?1u:0u);         // dai tensione al driver motore
}

static inline bool _laseren_read(void){
    return  _bit(NCR_PORT_LASEREN, NCR_BIT_LASEREN);
}
static inline void _laseren_write(bool on){
    gioSetBit(NCR_PORT_LASEREN, NCR_BIT_LASEREN, on?1u:0u);         // abilita uscita del laser
}

/* --- API --- */
void BoardPwr_Init(BoardPwrCtx* c)
{

    _vmoten_write(false);                           // Forza VMOTEN=0 (motori disabilitati, safe-state)

    c->st            = BoardPwr_ReadStatus();       // Legge un primo snapshot di BoardPwrStatus
    c->power_ok      = false;
    c->laser_ok      = false;
    c->fault_latched = false;
    c->ok_cnt        = 0u;
    c->bad_cnt       = 0u;
}

void BoardPwr_EnableVmot(bool on){
    _vmoten_write(on);
}

void Laser_Enable(bool on){
    _laseren_write(on);
}

BoardPwrStatus BoardPwr_ReadStatus(void)
{
    // Legge i tre ingressi che da LTC3350 vanno a RM57 e legge VMOTEN
    BoardPwrStatus s;
    s.ups_alert     = _ups_alert_asserted();
    s.input_present = !_ip_removed_asserted();
    s.cap_good      = _cap_good();
    s.vmot_enabled  = _vmoten_read();
    s.laser_enabled = _laseren_read();
    return s;
}

/* Policy: power_ok quando (input_present && cap_good && !ups_alert) stabile per BPWR_OK_DEBOUNCE_MS.
   Se degrada per BPWR_BAD_DEBOUNCE_MS -> power_ok=false, VMOTEN=0, fault_latched=true. */

void BoardPwr_Tick1kHz(BoardPwrCtx* c)
{
    BoardPwrStatus s = BoardPwr_ReadStatus();
    c->st = s;

    bool want_ok = (s.input_present && !s.ups_alert);                   // Vogliamo abilitare VMOT solo se l’ingresso 12 V è presente e non c’è un alert UPS

    if (want_ok){                                                       // Ingresso stabile + nUPS_ALERT=1 (nessun alert) per >= 50 ms
        if (c->ok_cnt < BPWR_OK_DEBOUNCE_MS) c->ok_cnt++;
        c->bad_cnt = 0u;

        if (!c->power_ok && c->ok_cnt >= BPWR_OK_DEBOUNCE_MS) {
            /* rising edge: diventiamo OK -> abilita VMOT */
            c->power_ok = true;
            _vmoten_write(true);

            if(BoardPwr_IsFaultLatched(c)) BoardPwr_ClearFault(c);      // azzeramento automatico del latch al recupero
        }
    } else {                                                            // Drop di ingresso o alert che dura >= 5 ms
        if (c->bad_cnt < BPWR_BAD_DEBOUNCE_MS) c->bad_cnt++;
        c->ok_cnt = 0u;

        if (c->power_ok && c->bad_cnt >= BPWR_BAD_DEBOUNCE_MS) {
            /* falling edge: degrado -> disabilita VMOT e latch fault */
            c->power_ok = false;
            c->fault_latched = true;                                    // Il latch di fault rimane a true finché non chiami BoardPwr_ClearFault()
            _vmoten_write(false);
        }
    }
}

void BoardPwr_ClearFault(BoardPwrCtx* c)
{
    c->fault_latched = false;
}

/* Utility/Getter */
bool BoardPwr_IsPowerOk(const BoardPwrCtx* c){
    return c->power_ok;
}
bool BoardPwr_IsFaultLatched(const BoardPwrCtx* c){
    return c->fault_latched;
}
