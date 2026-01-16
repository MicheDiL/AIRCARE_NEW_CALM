/*
 * uart.c
 *
 *  Created on: 3 nov 2025
 *      Author: mdilucchio
 */

#include "debug/uart.h"

static inline uint16_t tlm_crc(const TelemetryPkt* p){
    return (uint16_t)(p->hdr ^ p->a ^ p->b);
}

/* clamp helper di sicurezza contro overflow*/
static inline uint16_t clamp_u16(int32_t x){
    if (x < 0) return 0;
    if (x > 65535) return 65535;
    return (uint16_t)x;
}
static inline int16_t clamp_s16(int32_t x){
    if (x < -32768) return -32768;
    if (x >  32767) return  32767;
    return (int16_t)x;
}

static uint16_t s_period_pos = 20;   // ms
static uint16_t s_period_cur = 20;   // ms
static uint32_t s_next_pos_ms = 0;
static uint32_t s_next_cur_ms = 0;

extern uint32_t DrvRti_Millis(void);  // dal tuo Drv_Rti

void Uart_Init(uint16_t period_pos_ms, uint16_t period_cur_ms)
{
    s_period_pos = (period_pos_ms == 0) ? 1 : period_pos_ms;
    s_period_cur = (period_cur_ms == 0) ? 1 : period_cur_ms;
    s_next_pos_ms = s_next_cur_ms = 0;
}

/* ===== Pacchetto posizioni in LSB (target, enc1) ===== */
void Uart_TrySendPos(uint16_t target_lsb, uint16_t enc1_lsb)
{
    uint32_t now = DrvRti_Millis();         // Legge il “millis” corrente
    if (now < s_next_pos_ms) return;            // Se non è passato period_ms dall’ultima TX, esce subito => anche se la chiami ogni ms, invierai al più 1 pacchetto ogni period_ms

    // Impacchettamento (8 byte)
    TelemetryPkt pkt;
    pkt.hdr     = TLM_HDR_POS;
    pkt.a       = target_lsb;
    pkt.b       = enc1_lsb;
    pkt.crc     = tlm_crc(&pkt);

    (void)DrvSci_Write((const uint8_t*)&pkt, sizeof(pkt));  // enqueue degli 8B in un ring buffer in RAM (drop se pieno)
    s_next_pos_ms = now + s_period_pos;
}

/* ===== Pacchetto correnti in LSB (acm, pcm) ===== */
void Uart_TrySendCur(uint16_t acm_lsb, uint16_t pcm_lsb)
{
    uint32_t now = DrvRti_Millis();
    if (now < s_next_cur_ms) return;

    TelemetryPkt pkt;
    pkt.hdr = TLM_HDR_CUR;
    pkt.a   = acm_lsb;   /* IPROPI (magnitude) */
    pkt.b   = pcm_lsb;   /* INA240 (bipolare attorno a ~2048) */
    pkt.crc = tlm_crc(&pkt);

    (void)DrvSci_Write((const uint8_t*)&pkt, sizeof(pkt));
    s_next_cur_ms = now + s_period_cur;
}

/* ===== Pacchetto correnti in mA  =====
   - ACM: mA unsigned -> range utile: 0…65535 mA ma in pratica starai nell’ordine di 0..2000 mA

   - PCM: mA signed -> bias 32768:   enc = (int16_t)mA + 32768
           int16_t copre -32768…+32767 => somamndo 32768 al valore signed int16_t, trasli tutta la scala in un uint16_t così che:
            - il più negativo (-32768) diventi 0,
            - lo zero diventi 32768,
            - il massimo (+32767) diventi 65535.
    È una trasformazione lineare, monotona, biunivoca: non perdi informazione, cambi solo il riferimento dello zero.
*/
void Uart_TrySendCur_mA(uint16_t acm_mA_u16, int16_t pcm_mA_s16)
{
    uint32_t now = DrvRti_Millis();
    if (now < s_next_cur_ms) return;

    /* clamp & bias */
    uint16_t acm_enc = clamp_u16((int32_t)acm_mA_u16);              // unsigned mA
    int16_t  pcm_cl  = clamp_s16((int32_t)pcm_mA_s16);              // il cast a int32_t prima della somma evita overflow nei calcoli intermedi
    uint16_t pcm_enc = (uint16_t)((int32_t)pcm_cl + 32768);         // offset-binary

    TelemetryPkt pkt;
    pkt.hdr = TLM_HDR_CUR;
    pkt.a   = acm_enc;
    pkt.b   = pcm_enc;
    pkt.crc = tlm_crc(&pkt);

    (void)DrvSci_Write((const uint8_t*)&pkt, sizeof(pkt));
    s_next_cur_ms = now + s_period_cur;
}

void Uart_TrySendCurSigned_mA(int16_t iref_mA_s16, int16_t pcm_mA_s16)
{
    uint32_t now = DrvRti_Millis();
    if (now < s_next_cur_ms) return;

    /* clamp & bias */
    int16_t  iref_cl   = clamp_s16((int32_t)iref_mA_s16);              // unsigned mA
    uint16_t iref_enc = (uint16_t)((int32_t)iref_cl + 32768);
    int16_t  pcm_cl   = clamp_s16((int32_t)pcm_mA_s16);              // il cast a int32_t prima della somma evita overflow nei calcoli intermedi
    uint16_t pcm_enc  = (uint16_t)((int32_t)pcm_cl + 32768);         // offset-binary

    TelemetryPkt pkt;
    pkt.hdr = TLM_HDR2_CUR;
    pkt.a   = iref_enc;
    pkt.b   = pcm_enc;
    pkt.crc = tlm_crc(&pkt);

    (void)DrvSci_Write((const uint8_t*)&pkt, sizeof(pkt));
    s_next_cur_ms = now + s_period_cur;
}

