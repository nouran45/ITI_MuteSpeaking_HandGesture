/*
 * UART_Register.h
 *
 *  Created on: Sep 7, 2025
 *      Author: yasser
 */

#ifndef UART_REGISTER_H_
#define UART_REGISTER_H_

/* Data Register */
#define UDR     (*(volatile u8*)0x2C)

/* Control and Status Register A */
#define UCSRA   (*(volatile u8*)0x2B)
#define RXC     7   /* Receive Complete */
#define TXC     6   /* Transmit Complete */
#define UDRE    5   /* Data Register Empty */
#define FE      4   /* Frame Error */
#define DOR     3   /* Data Overrun */
#define PE      2   /* Parity Error */
#define U2X     1   /* Double Speed */
#define MPCM    0   /* Multi-processor Communication Mode */

/* Control and Status Register B */
#define UCSRB   (*(volatile u8*)0x2A)
#define RXCIE   7   /* RX Complete Interrupt Enable */
#define TXCIE   6   /* TX Complete Interrupt Enable */
#define UDRIE   5   /* Data Register Empty Interrupt Enable */
#define RXEN    4   /* Receiver Enable */
#define TXEN    3   /* Transmitter Enable */
#define UCSZ2   2   /* Character Size bit 2 */
#define RXB8    1   /* Receive Data Bit 8 (9-bit) */
#define TXB8    0   /* Transmit Data Bit 8 (9-bit) */

/* Control and Status Register C
 */
#define UCSRC   (*(volatile u8*)0x40)
#define URSEL   7   /* USART Register Select (1=UCSRC, 0=UBRRH) */

/* ------------- UCSRC bit positions (when URSEL=1) ------------- */
#define UMSEL   6   /* Mode Select: 0=Async, 1=Sync */
#define UPM1    5   /* Parity Mode bit1 */
#define UPM0    4   /* Parity Mode bit0 */
#define USBS    3   /* Stop Bit Select: 0=1 stop, 1=2 stops */
#define UCSZ1   2   /* Character Size bit1 */
#define UCSZ0   1   /* Character Size bit0 */
#define UCPOL   0   /* Clock Polarity (sync mode) */

/* Baud Rate Registers */
#define UBRRH   (*(volatile u8*)0x40)  /* URSEL=0 to access */
#define UBRRL   (*(volatile u8*)0x29)
#endif /* UART_REGISTER_H_ */
