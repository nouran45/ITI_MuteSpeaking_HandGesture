#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "I2C_Interface.h"
#include "I2C_Config.h"
#include "../DIO/DIO_Register.h"  // For port registers
#include <util/delay.h>  // For _delay_us function

// TWI (I2C) Registers
// For ATmega32A, the TWI registers are different:
#define TWBR *((volatile u8*)0x20)
#define TWSR *((volatile u8*)0x21)
#define TWAR *((volatile u8*)0x22)
#define TWDR *((volatile u8*)0x23)
#define TWCR *((volatile u8*)0x56)



// TWCR Bits
#define TWINT 7
#define TWEA  6
#define TWSTA 5
#define TWSTO 4
#define TWWC  3
#define TWEN  2
#define TWIE  0

void I2C_voidInit(void) {
    // Configure I2C pins for ATmega32a
    // PC0 = SDA, PC1 = SCL
    // Set as inputs with pull-ups enabled
    DDRC &= ~((1 << 0) | (1 << 1));  // PC0 and PC1 as inputs
    PORTC |= (1 << 0) | (1 << 1);    // Enable pull-ups on PC0 and PC1
    
    // Simple I2C initialization without potential hanging
    
    // First, ensure I2C is disabled
    TWCR = 0x00;
    
    // Set bit rate register for 100kHz with 8MHz CPU
    // TWBR = ((F_CPU/SCL_CLOCK)-16)/(2*PRESCALER)
    // For 100kHz: TWBR = ((8000000/100000)-16)/(2*1) = 32
    TWBR = 32;
    
    // Set prescaler to 1 (TWPS1:TWPS0 = 00)
    TWSR = 0x00;
    
    // Enable I2C
    TWCR = (1 << TWEN);
    
    // Small delay to ensure initialization
    _delay_us(10);
}

void I2C_voidStart(void) {
    // Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    // Wait for TWINT flag set with timeout
    u16 timeout = 10000;
    while (!GET_BIT(TWCR, TWINT) && timeout--);
}

void I2C_voidStop(void) {
    // Send STOP condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_voidWrite(u8 u8Data) {
    // Load data into data register
    TWDR = u8Data;
    
    // Clear TWINT bit to start transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag set with timeout
    u16 timeout = 10000;
    while (!GET_BIT(TWCR, TWINT) && timeout--);
}

u8 I2C_u8ReadWithACK(void) {
    // Enable ACK and clear TWINT to start reception
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    
    // Wait for TWINT flag set with timeout
    u16 timeout = 10000;
    while (!GET_BIT(TWCR, TWINT) && timeout--);
    
    // Return received data
    return TWDR;
}

u8 I2C_u8ReadWithNACK(void) {
    // Clear TWINT to start reception (no ACK)
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag set with timeout
    u16 timeout = 10000;
    while (!GET_BIT(TWCR, TWINT) && timeout--);
    
    // Return received data
    return TWDR;
}

u8 I2C_u8GetStatus(void) {
    // Mask status (upper 5 bits of TWSR)
    return (TWSR & 0xF8);
}