#ifndef DIO_REGISTER_H
#define DIO_REGISTER_H

#include "STD_TYPES.h"

// PORT A Registers
#define PORTA *((volatile u8 *)(0x3B))
#define DDRA  *((volatile u8 *)(0x3A))
#define PINA  *((volatile u8 *)(0x39))

// PORT B Registers
#define PORTB *((volatile u8 *)(0x38))
#define DDRB  *((volatile u8 *)(0x37))
#define PINB  *((volatile u8 *)(0x36))

// PORT C Registers
#define PORTC *((volatile u8 *)(0x35))
#define DDRC  *((volatile u8 *)(0x34))
#define PINC  *((volatile u8 *)(0x33))

// PORT D Registers
#define PORTD *((volatile u8 *)(0x32))
#define DDRD  *((volatile u8 *)(0x31))
#define PIND  *((volatile u8 *)(0x30))

// UART Registers
#define UBRRH *((volatile u8*)0x40)
#define UBRRL *((volatile u8*)0x29)
#define UCSRA *((volatile u8*)0x2B)
#define UCSRB *((volatile u8*)0x2A)
#define UCSRC *((volatile u8*)0x40)
#define UDR   *((volatile u8*)0x2C)

// I2C/TWI Registers
#define TWBR *((volatile u8*)0x20)
#define TWSR *((volatile u8*)0x21)
#define TWAR *((volatile u8*)0x22)
#define TWDR *((volatile u8*)0x23)
#define TWCR *((volatile u8*)0x56)

// TWI Control Register Bits
#define TWINT 7
#define TWEA  6
#define TWSTA 5
#define TWSTO 4
#define TWWC  3
#define TWEN  2
#define TWIE  0

// TWI Status Mask
#define TWSR_STATUS_MASK 0xF8

#endif /* DIO_REGISTER_H */
