/*
 * SOFT_UART.h  — Software UART for ATmega32 @ 8 MHz (default)
 * TX: PD2  (idle HIGH)
 * RX: PD3 / INT1 (with pull-up)
 *
 * Features:
 *  - Non-blocking receive helper (like UART_u8ReceiveNoBlock)
 *  - Optional wrapper to replace your HW UART API:
 *      #define USE_SOFT_UART_AS_UART0 1
 *    …then your calls to UART_voidInit / UART_voidSendChar / UART_u8ReceiveNoBlock
 *    will be routed to this soft UART automatically.
 */

#ifndef SOFT_UART_H_
#define SOFT_UART_H_

#include "STD_TYPES.h"
#include "DIO_Register.h"
#include "UART_Register.h"
#include <stdint.h>

/* ====== Build-time configuration ====== */
#ifndef F_CPU
# define F_CPU 8000000UL
#endif

#ifndef SOFTUART_BAUD
# define SOFTUART_BAUD    9600UL
#endif

/* Pins (ATmega32) */
#ifndef SOFTUART_TX_PORT
# define SOFTUART_TX_PORT PORTD
# define SOFTUART_TX_DDR  DDRD
# define SOFTUART_TX_PIN  PD2
#endif

#ifndef SOFTUART_RX_PIN
# define SOFTUART_RX_PIN  PD3      /* INT1 */
#endif

/* ====== Public API ====== */
void SOFT_UART_voidInit(void);
void SOFT_UART_voidPutChar(u8 c);
void SOFT_UART_voidPutString(const char *s);

/* Non-blocking: returns 1 if a byte was read into *out, else 0 */
u8    SOFT_UART_u8GetCharNoBlock(u8 *out);

/* Blocking: waits until a byte is available, returns it */
u8    SOFT_UART_u8GetCharBlocking(void);

/* Available bytes in RX buffer */
u8    SOFT_UART_u8Available(void);

/* ====== Optional wrapper to match your HW UART API ====== */
/* Enable this if you want to keep the rest of your code unchanged */
#ifdef USE_SOFT_UART_AS_UART0

/* Map your hardware UART functions to soft UART */
#define UART_voidInit()               SOFT_UART_voidInit()
#define UART_voidSendChar(ch)         SOFT_UART_voidPutChar((u8)(ch))

/* Non-blocking receive: returns 1 if a byte was read into *p, else 0 */
static inline u8 UART_u8ReceiveNoBlock(u8 *p) {
    return SOFT_UART_u8GetCharNoBlock(p);
}

#endif /* USE_SOFT_UART_AS_UART0 */

#endif /* SOFT_UART_H_ */
