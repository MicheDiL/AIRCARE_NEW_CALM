/*
 * app_control_defaults.c
 *
 *  Created on: 31 ott 2025
 *      Author: mdilucchio
 */

#include "plant/plant_control.h"

/* Base cfg: i dead-zone verranno riempiti dalla calibrazione runtime */
static const PlantControlCfg kPlant_Base = {
    .Vmot_V                 = 12.0f,    // Vmot_V alla rail reale VMOT (dipende da quale Jumper viene utilizzato. COn JP8 allora VMOT = 5V con JP9 allora VMOT da VSYS->VMOTSW ~12V)
    .e_band_enter_lsb       = 18u,
    .e_band_exit_lsb        = 22u,
    .vel_band_lsb_per_ms    = 2.0f,
    .dwell_ms               = 200u,

    .e_limit_lsb            = 600u,         // NEW: ERROR LIMITER 0.483 V @ 3.3V/4095 (~27% dello span POSX ~2200 LSB)

    .R_coil_Ohm             = 3.2f,         // NEW: CONTROLLO IN CORRENTE
    .Imax_A                 = 1.5f,         // NEW: CONTROLLO IN CORRENTE
    .Imin_A                 = 0.03f,        // NEW: CONTROLLO IN CORRENTE (sotto 30 mA: trattalo come zero)

    .dir_to_increase        = ACT_DIR_REV,  // ACT_DIR_FWD,
    .decay_mode             = DECAY_BRAKE,
};

const PlantControlCfg* PlantControl_BaseCfg(void)
{
    return &kPlant_Base; /* const -> non modificabile dall’esterno */
}
