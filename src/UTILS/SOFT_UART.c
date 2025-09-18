/*
 * SOFT_UART.c — Software UART implementation
 * ATmega32 @ 8 MHz (default), 9600 bps (default)
 *
 * Uses:
 *  - Timer1 in CTC mode (Compare A)
 *  - External Interrupt INT1 for start-bit detect (PD3)
 *
 * NOTE: Make sure INT1 and Timer1 are not used by other modules.
 */

#include "SOFT_UART.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* ====== Timer and baud config ====== */
#define SOFTUART_TIMER_PRESC      8UL
/* Tick = F_CPU / presc -> bit time = 1/BAUD seconds => OCR = round(ticks_per_bit) - 1 */
static inline uint16_t softuart_ocr_from_baud(void) {
    /* rounded integer: ((F_CPU/(presc*BAUD)) - 1) with rounding */
    uint32_t denom = SOFTUART_TIMER_PRESC * SOFTUART_BAUD;
    uint32_t ticks = (F_CPU + (denom/2)) / denom;     /* round to nearest */
    if (ticks == 0) ticks = 1;
    return (uint16_t)(ticks - 1);
}

/* ====== Ring buffer ====== */
#define SOFTUART_RX_BUFSZ   64

static volatile u8  rx_buf[SOFTUART_RX_BUFSZ];
static volatile u8  rx_head = 0, rx_tail = 0;

/* ====== TX state ====== */
static volatile u8  tx_busy = 0;
static volatile u16 tx_shift_reg = 0;         /* start(0) + 8 data + stop(1) */
static volatile u8  tx_bits_remaining = 0;

/* ====== RX state ====== */
static volatile u8  rx_sampling = 0;          /* 1 while sampling data bits */
static volatile u8  rx_bit_index = 0;         /* 0..7 */
static volatile u8  rx_byte = 0;

/* ====== GPIO helpers ====== */
static inline void tx_high(void){ SOFTUART_TX_PORT |=  (1<<SOFTUART_TX_PIN); }
static inline void tx_low (void){ SOFTUART_TX_PORT &= ~(1<<SOFTUART_TX_PIN); }
static inline u8   rx_read(void){ return (PIND & (1<<SOFTUART_RX_PIN)) ? 1 : 0; }

/* ====== API ====== */
void SOFT_UART_voidInit(void)
{
    /* TX: output, idle HIGH */
    SOFTUART_TX_DDR |= (1<<SOFTUART_TX_PIN);
    tx_high();

    /* RX: PD3/INT1 input with pull-up */
    DDRD  &= ~(1<<SOFTUART_RX_PIN);
    PORTD |=  (1<<SOFTUART_RX_PIN);

    /* Timer1 CTC, prescaler = 8 */
    TCCR1A = 0;
    TCCR1B = (1<<WGM12) | (1<<CS11);
    OCR1A  = softuart_ocr_from_baud();        /* ~104 for 8MHz/9600 */

    /* Enable External Interrupt INT1 on falling edge (start bit) */
    MCUCR |= (1<<ISC11);      /* ISC11:ISC10 = 10 -> falling edge */
    MCUCR &= ~(1<<ISC10);
    GICR  |= (1<<INT1);       /* enable INT1 */
    GIFR  |= (1<<INTF1);      /* clear flag */

    /* Initially: timer interrupt disabled until TX or RX starts */
    TIMSK &= ~(1<<OCIE1A);

    /* Enable global interrupts */
    sei();
}

void SOFT_UART_voidPutChar(u8 c)
{
    /* Wait if previous byte still sending */
    while (tx_busy) { /* spin */ }

    /* Frame: start(0) LSB-first, 8 data, stop(1) */
    tx_shift_reg = (1u << 9) | ((u16)c << 1); /* bit0=start=0 -> we send LSB of reg first */
    tx_bits_remaining = 10;
    tx_busy = 1;

    /* Kick timer ISR */
    TCNT1 = 0;
    TIMSK |= (1<<OCIE1A);
}

void SOFT_UART_voidPutString(const char *s)
{
    while (*s) {
        SOFT_UART_voidPutChar((u8)*s++);
        /* small pacing is safe; adjust if needed */
        _delay_ms(2);
    }
}

u8 SOFT_UART_u8Available(void)
{
    return (rx_head != rx_tail);
}

u8 SOFT_UART_u8GetCharNoBlock(u8 *out)
{
    if (rx_head == rx_tail) return 0;
    *out = rx_buf[rx_tail];
    rx_tail = (u8)((rx_tail + 1) % SOFTUART_RX_BUFSZ);
    return 1;
}

u8 SOFT_UART_u8GetCharBlocking(void)
{
    while (!SOFT_UART_u8Available()) { /* spin */ }
    u8 c = rx_buf[rx_tail];
    rx_tail = (u8)((rx_tail + 1) % SOFTUART_RX_BUFSZ);
    return c;
}

/* ====== ISRs ====== */
/* Timer1 Compare A: drives TX bit timing and RX sampling */
ISR(TIMER1_COMPA_vect)
{
    /* ---- TX ---- */
    if (tx_busy) {
        if (tx_bits_remaining) {
            if (tx_shift_reg & 0x01u) tx_high(); else tx_low();
            tx_shift_reg >>= 1;
            tx_bits_remaining--;
        } else {
            /* done */
            tx_busy = 0;
            tx_high(); /* idle */
            /* if not receiving, we can disable timer interrupt to save power */
            if (!rx_sampling) TIMSK &= ~(1<<OCIE1A);
        }
    }

    /* ---- RX ---- */
    if (rx_sampling) {
        if (rx_bit_index < 8) {
            u8 bit = rx_read();
            rx_byte |= (u8)(bit << rx_bit_index);
            rx_bit_index++;
        } else {
            /* stop bit — ignore value, just store the byte */
            u8 next = (u8)((rx_head + 1) % SOFTUART_RX_BUFSZ);
            if (next != rx_tail) {
                rx_buf[rx_head] = rx_byte;
                rx_head = next;
            }
            rx_sampling = 0;
            rx_bit_index = 0;
            rx_byte = 0;
            /* If not transmitting, stop timer ISR until next frame */
            if (!tx_busy) TIMSK &= ~(1<<OCIE1A);
        }
    }
}

/* INT1: detect start bit (falling edge) and schedule mid-bit sampling */
ISR(INT1_vect)
{
    /* Start bit detected (line LOW) */
    if (!rx_sampling && !rx_read()) {
        rx_sampling = 1;
        rx_bit_index = 0;
        rx_byte = 0;

        /* schedule first data-bit sample at half bit-time from now */
        /* set TCNT so the next OCR1A match occurs after ~0.5 bit-time */
        uint16_t half = (uint16_t)((softuart_ocr_from_baud() + 1) / 2);
        TCNT1 = half;

        /* enable timer interrupt */
        TIMSK |= (1<<OCIE1A);
    }
}
