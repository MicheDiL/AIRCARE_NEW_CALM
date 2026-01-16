/*
 * app_calibrator.c
 *
 *  Created on: 3 nov 2025
 *      Author: mdilucchio
 */

#include "app/app_calibrator.h"
//#include "controller/ctrl_types.h" // ADC_FS_LSB, ADC_VREF_V
#include <math.h>


/* ===== default in ROM + accessor ===== */
static const AppCalibCfg kCalib_Default = {
    .vstep_V        = 0.09f,
    .hold_ms        = 200u,
    .settle_ms      = 2000u,
    .delta_lsb_thr  = 35u,
    .vmot_V         = 12.0f //9.0f
};

const AppCalibCfg* AppCalib_DefaultCfg(void){
    return &kCalib_Default;
}

/* ===== API ===== */
void AppCalib_Init(AppCalibCtx* c){
    c->st               = CAL_INIT;
    c->hold_cnt         = 0u;
    c->settle_cnt       = 0u;
    c->y0_pos           = 0u;
    c->y0_neg           = 0u;
    c->delta_pos_adc    = 0u;
    c->delta_neg_adc    = 0u;
    c->v_cmd            = 0.0f;
    c->vdead_pos_V      = 0.0f;
    c->vdead_neg_V      = 0.0f;
    c->done             = false;
}

void AppCalib_Reset(AppCalibCtx* c){
    AppCalib_Init(c);
}

void AppCalib_Step1kHz(AppCalibCtx* c,
                       const AppCalibCfg* cfg,
                       uint16_t y_adc,
                       AppApplyVoltFn applyVolt,
                       void* user)
{
    if (c->done) {
        if (applyVolt) applyVolt(user, 0.0f); return;
    }

    switch (c->st)
    {
    case CAL_INIT:
        c->v_cmd = 0.0f;
        if (applyVolt) applyVolt(user, 0.0f);
        c->hold_cnt = c->settle_cnt = 0u;
        c->st = CAL_POS_APPLY;
        break;

    case CAL_POS_APPLY:
        c->v_cmd = sat(c->v_cmd + cfg->vstep_V, 0.0f, cfg->vmot_V);
        if (applyVolt) applyVolt(user, c->v_cmd);
        c->y0_pos = y_adc;
        c->hold_cnt = 0u;
        c->st = CAL_POS_HOLD;
        break;

    case CAL_POS_HOLD:
        if (++c->hold_cnt >= cfg->hold_ms) c->st = CAL_POS_CHECK;
        break;

    case CAL_POS_CHECK:
        c->delta_pos_adc = (uint16_t)abs((int)y_adc - (int)c->y0_pos);
        if (c->delta_pos_adc >= cfg->delta_lsb_thr || c->v_cmd >= cfg->vmot_V) {
            c->vdead_pos_V = c->v_cmd;
            if (applyVolt) applyVolt(user, 0.0f);
            c->settle_cnt = 0u;
            c->st = CAL_ZERO_SETTLE;
        } else {
            c->st = CAL_POS_APPLY;
        }
        break;

    case CAL_ZERO_SETTLE:
        if (applyVolt) applyVolt(user, 0.0f);
        if (++c->settle_cnt >= cfg->settle_ms) {
            c->v_cmd = 0.0f;
            c->hold_cnt = 0u;
            c->st = CAL_NEG_APPLY;
        }
        break;

    case CAL_NEG_APPLY:
        c->v_cmd = sat(c->v_cmd - cfg->vstep_V, -cfg->vmot_V, 0.0f);
        if (applyVolt) applyVolt(user, c->v_cmd);
        c->y0_neg = y_adc;
        c->hold_cnt = 0u;
        c->st = CAL_NEG_HOLD;
        break;

    case CAL_NEG_HOLD:
        if (++c->hold_cnt >= cfg->hold_ms) c->st = CAL_NEG_CHECK;
        break;

    case CAL_NEG_CHECK:
        c->delta_neg_adc = (uint16_t)abs((int)y_adc - (int)c->y0_neg);
        if (c->delta_neg_adc >= cfg->delta_lsb_thr || c->v_cmd <= -cfg->vmot_V) {
            c->vdead_neg_V = fabsf(c->v_cmd);
            if (applyVolt) applyVolt(user, 0.0f);
            c->st = CAL_DONE;
        } else {
            c->st = CAL_NEG_APPLY;
        }
        break;

    case CAL_DONE:
    default:
        c->done = true;
        if (applyVolt) applyVolt(user, 0.0f);
        break;
    }
}
