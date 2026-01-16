/*
 * drv_sci.c
 *
 *  Created on: 4 nov 2025
 *      Author: mdilucchio
 */

#include "debug/drv_sci.h"


/*============================ TX ring ============================ */
static volatile uint8_t  s_tx_ring[SCI_TX_RING_SIZE];      // Buffer circolare di 512 byte (da 0 a 511) per la TX -> il buffer è pieno quando (head+1) % N == tail
static volatile uint16_t s_tx_head = 0;                    // indice dove il producer mette il prossimo byte: Producer = chi chiama DrvSci_Write        -> incrementa head
static volatile uint16_t s_tx_tail = 0;                    // indice dove il consumer estrae il prossimo byte: Consumer = ISR TX (DrvSci_TxIsrService)     -> incrementa tail
static volatile uint32_t s_tx_drop = 0;                    // Contatore di byte scartati quando il ring è pieno

// stato di una trasmissione HAL “in corso” avviata con sciSend()
static volatile uint16_t s_tx_len_inflight = 0;            // lunghezza del blocco che è stato consegnato a sciSend() e che HAL sta ancora trasmettendo in ISR. È >0 finché non ricevi SCI_TX_INT di fine blocco.

/*============================ RX ring ============================ */
/* Idea: la UART riceve byte a ritmo di baud e tu non puoi permetterti di fare parsing nel mezzo del loop di controllo. Quindi l’ISR “parcheggia” i byte in RAM, e il main li “processa” quando vuole. */
static volatile uint8_t  s_rx_ring[SCI_RX_RING_SIZE];     // memoria dei byte ricevuti da Qt
static volatile uint16_t s_rx_head = 0;                   // indice producer (ISR RX): dove scrivere il prossimo byte ricevuto
static volatile uint16_t s_rx_tail = 0;                   // indice consumer (main): da dove leggere il prossimo byte.
static volatile uint32_t s_rx_drop = 0;                   // incrementa quando ISR riceve byte ma il ring è pieno (quindi drop).

/* FIX RXINT: Byte di inizializzazione per la ricezione HALCoGen in interrupt-mode (1 byte alla volta) */
static uint8_t s_rx_byte;
/********************************************* Helpers TX **********************************************/
static inline uint16_t tx_ring_free(void){
    uint16_t h = s_tx_head, t = s_tx_tail;
    return (uint16_t)((t + SCI_TX_RING_SIZE - h - 1) % SCI_TX_RING_SIZE);   // ritona quanti byte puoi ancora inserire nel buffer ad anello s_rx_ring
}

static inline bool tx_ring_put(uint8_t b){                             // accoda un byte; fallisce se il ring è pieno
    uint16_t nxt = (uint16_t)((s_tx_head + 1u) % SCI_TX_RING_SIZE);    // Calcola l’indice successivo circolare: 0->1, 1->2,...,511->0
    if (nxt == s_tx_tail) return false;                                // buffer pieno -> non scrivere
    s_tx_ring[s_tx_head] = b;                                             // Memorizza il byte nella cella corrente indicata da s_head
    s_tx_head = nxt;                                                   // Avanza head alla posizione successiva
    return true;
}

// restituisce un chunk lineare [ptr,len] fino a wrap (len=0 se vuoto)
static uint16_t tx_ring_peek_linear(const uint8_t** ptr){
    if (s_tx_tail == s_tx_head) {
        *ptr = NULL;
        return 0;
    }
    if (s_tx_head > s_tx_tail) {
        *ptr = &s_tx_ring[s_tx_tail];
        return (uint16_t)(s_tx_head - s_tx_tail);
    } else {
        *ptr = &s_tx_ring[s_tx_tail];
        return (uint16_t)(SCI_TX_RING_SIZE - s_tx_tail); // fino a fine buffer
    }
}

static void tx_ring_consume(uint16_t n){
    s_tx_tail = (uint16_t)((s_tx_tail + n) % SCI_TX_RING_SIZE);
}

// avvia una TX se idle: prende un chunk lineare dal ring e chiama sciSend()
static void kick_if_idle(void){
    if (s_tx_len_inflight != 0) return;              // già in corso

    const uint8_t* ptr;
    uint16_t len = tx_ring_peek_linear(&ptr);

    if (len == 0) return;                             // nessuna trasmissione in corso

    /* IMPORTANTISSIMO: sciSend è non bloccante in modalità interrupt */
    sciSend(sciREGx /*sciREG3*/, len, (uint8*)ptr);

    s_tx_len_inflight = len;                          // ricordiamo quanto abbiamo chiesto di spedire

    // Da qui in poi, ogni volta che l’hardware è pronto per il prossimo byte, scatterà un interrupt. Questo finché il conteggio tx_length non arriva a zero
}

/*static inline bool ring_get(uint8_t* out){
    if (s_tx_tail == s_tx_head) return false;                             // buffer vuoto -> nulla da estrarre
    *out = s_tx_ring[s_tx_tail];                                         // estrae il byte alla coda del ring buffer
    s_tx_tail = (uint16_t)((s_tx_tail + 1u) % SCI_TX_RING_SIZE);          // avanza tail
    return true;
}*/

/* ============================ Helpers RX ============================ */
static inline bool rx_ring_put(uint8_t b)
{
    uint16_t nxt = (uint16_t)((s_rx_head + 1u) % SCI_RX_RING_SIZE); // calcola la prossima posizione del buffer RX circolare
    if (nxt == s_rx_tail) // condizione di ring RX pieno
    {
        s_rx_drop++;
        return false;   // ring pieno
    }
    s_rx_ring[s_rx_head] = b;   // scrive il byte ricevuto nella posizione s_rx_head
    s_rx_head = nxt;            // avanza il producer
    return true;                // segnala "byte enqueued"
}

static inline bool rx_ring_get(uint8_t* out)
{
    if (s_rx_head == s_rx_tail) return false;   // ring vuoto
    *out = s_rx_ring[s_rx_tail];                // legge il byte
    s_rx_tail = (uint16_t)((s_rx_tail + 1u) % SCI_RX_RING_SIZE); // avanza il consumer
    return true; // il byte è stato letto
}

static inline uint16_t rx_ring_used(void){
    uint16_t h = s_rx_head, t = s_rx_tail;
    return (uint16_t)((h + SCI_RX_RING_SIZE - t) % SCI_RX_RING_SIZE);
}

/********************************************* API ************************************************/
void DrvSci_Init(void)
{
    sciInit();                                   // HALCoGen setup (baud, frame, pinmux)

    /* abilita IRQ TX + RX */
    sciEnableNotification(sciREGx, SCI_TX_INT);  // abilita TX interrupt
    sciEnableNotification(sciREGx, SCI_RX_INT);  // abilita RX interrupt

    // Inzializza gli indici dei buffer per TX
    s_tx_head = s_tx_tail = 0;
    s_tx_drop = 0;
    s_tx_len_inflight = 0;

    // Inizializza gli indici dei buffer RX
    s_rx_head = s_rx_tail = 0;
    s_rx_drop = 0;

    /* FINX RXINT:
           arma la ricezione in interrupt-mode, 1 byte alla volta,
           così sciNotification riceve davvero SCI_RX_INT.
           L'idea è quando ricevi 1 byte mettilo in s_rx_byte così che può partire la chiamata a sciNotification */
        sciReceive(sciREGx, 1u, &s_rx_byte);
}

bool DrvSci_Write(const uint8_t* data, uint16_t len)    //  enqueue: Prova ad accodare len byte nel ring
{
    /*La firma di questa funzione vuol significa: “Dammi l’indirizzo del primo byte e quanti byte devo mandare”
     * Il driver della seriale deve lavorae con bytes (uint8_t) perchè qualsiasi informazione tu voglia spedire
     * (numeri a 16 bit, struct, float, ecc.) deve alla fine diventare una sequenza di byte. */

    // Guard-rail
    if (!data || len == 0u) return true;

    uint16_t i;

    //se non c'è spazio, drop tutto il pacchetto: evita di trasmettere mezzi frame
    if (tx_ring_free() < len) {
        s_tx_drop++;
        return false;
    }

    for (i = 0; i < len; ++i) {
        tx_ring_put(data[i]);                              // 8 byte entrano nel buffer circolare
    }

    /* Dato che kick_if_idle() viene chiamata anche nella ISR allora bisogna impedire che ISR chiami kik_if_idle() quando questa è già stata chiamata da DrvSci_Write*/
    _disable_IRQ_interrupt_();
    kick_if_idle();   // se non c’è una trasmissione TX in corso (s_tx_len_inflight == 0), sciSend() partirà e scrive subito il primo byte nel registro TD (questo è il “kick” hardware del blocco)
    _enable_IRQ_interrupt_();

    return true;
}

uint32_t DrvSci_Dropped(void){
    return s_tx_drop;       // restituisce il contatore drop TX
}

/* ===== RX API ===== */
uint16_t DrvSci_RxAvailable(void)
{
    return rx_ring_used(); // quanti byte sono in coda
}

bool DrvSci_ReadByte(uint8_t* out)
{
    if (!out) return false;
    return rx_ring_get(out);    // consuma 1 byte dal ring RX
}

uint16_t DrvSci_Read(uint8_t* buf, uint16_t maxlen)
{
    if (!buf || maxlen == 0u) return 0u;

    uint16_t n = 0; // contatore dei byte letti
    while (n < maxlen) {
        uint8_t b;
        if (!rx_ring_get(&b)) break;    // prova a leggere ma se il ring RX è vuoto allora break
        buf[n++] = b;                   // copia il byte
    }
    return n;                           // ritorna quanti byte sono stati letti nel ring RX
}

uint32_t DrvSci_RxDropped(void)
{
    return s_rx_drop;       // restituisce il contatore drop RX
}

/* ============================ ISR callback HALCoGen ============================ */
// Quando l’hardware smette di mandare il blocco di 8byte, HALCoGen chiama questa funzione
void sciNotification(sciBASE_t *sci, uint32 flags)
{
    (void)sci;

    /* RX: leggi i byte disponibili e buttali nel ring RX
           Con sciReceive() HALCoGen ti chiama quando il byte è stato ricevuto.
           Il byte è già in s_rx_byte. */
    if ((flags & SCI_RX_INT) != 0u) {
        (void)rx_ring_put(s_rx_byte); // parcheggio il byte ricevuto in sx_ring_put

        /* ri-arma subito per il prossimo byte da ricevere */
        sciReceive(sciREGx, 1u, &s_rx_byte);
    }

    /* (opzionale ma consigliato) gestisci errori RX */
    if ((flags & (SCI_FE_INT | SCI_OE_INT | SCI_PE_INT)) != 0u) {
        /* legge e clear degli error flags nel driver HALCoGen */
        (void)sciRxError(sciREGx);

        /* ri-arma comunque */
        sciReceive(sciREGx, 1u, &s_rx_byte);
    }

    /* TX: fine blocco -> consuma e kick successivo */
    if ((flags & SCI_TX_INT) != 0u) {
        /* Un blocco lanciato con sciSend() è appena finito: consumalo dal ring */
        if (s_tx_len_inflight) {
            tx_ring_consume(s_tx_len_inflight);    // togli dal ring il blocco appena inviato
            s_tx_len_inflight = 0;
        }
        // Se nel ring c’è altro, ri-avvii una nuova TX chiamando di nuovo sciSend() (nuovo “kick di blocco”).
        kick_if_idle();
    }
}
