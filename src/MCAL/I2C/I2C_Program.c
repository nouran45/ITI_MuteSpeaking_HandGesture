#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "I2C_Interface.h"
#include "I2C_Config.h"
#include "DIO_Interface.h"
#include "DIO_Register.h"  // For port registers
#include <util/delay.h>  // For _delay_us function

// CORRECT TWI Register Definitions for ATmega32
#define TWBR *((volatile u8*)0x20)
#define TWSR *((volatile u8*)0x21)
#define TWAR *((volatile u8*)0x22)
#define TWDR *((volatile u8*)0x23)
#define TWCR *((volatile u8*)0x56)

// TWCR Bit Definitions
#define TWINT   7
#define TWEA    6
#define TWSTA   5
#define TWSTO   4
#define TWWC    3
#define TWEN    2
#define TWIE    0


// TWSR Status Mask and Prescaler Bits
#define TWSR_STATUS_MASK 0xF8
#define TWPS1   1
#define TWPS0   0


// TWSR Status Mask
#define TWSR_STATUS_MASK 0xF8

// I2C Timeout
#define I2C_TIMEOUT 10000

// Helper functions (static/private)
static inline u8 twi_status(void) { 
    return (TWSR & 0xF8); 
}

static inline u8 SLA(u8 addr7, u8 rw) { 
    return (u8)((addr7 << 1) | (rw & 1)); 
}

static u8 hlp_start(u8 addr_rw) {
    // Send START
    TWCR = (1u<<TWINT) | (1u<<TWSTA) | (1u<<TWEN);
    while(!(TWCR & (1u<<TWINT)));
    u8 st = twi_status();
    if (st != 0x08 && st != 0x10) return 1; // START / REP START

    // Send SLA+R/W
    TWDR = addr_rw;
    TWCR = (1u<<TWINT) | (1u<<TWEN);
    while(!(TWCR & (1u<<TWINT)));
    st = twi_status();
    // SLA+W ACK(0x18) or SLA+R ACK(0x40)
    if (st != 0x18 && st != 0x40) return 1;

    return 0;
}

static inline void hlp_stop(void) {
    TWCR = (1u<<TWINT) | (1u<<TWEN) | (1u<<TWSTO);
    // No wait required; bus releases automatically
}

static u8 hlp_write(u8 data) {
    TWDR = data;
    TWCR = (1u<<TWINT) | (1u<<TWEN);
    while(!(TWCR & (1u<<TWINT)));
    // Data transmitted, ACK received => 0x28
    return (twi_status() == 0x28) ? 0 : 1;
}

static u8 hlp_readAck(void) {
    TWCR = (1u<<TWINT) | (1u<<TWEN) | (1u<<TWEA);
    while(!(TWCR & (1u<<TWINT)));
    return TWDR;
}

static u8 hlp_readNack(void) {
    TWCR = (1u<<TWINT) | (1u<<TWEN);
    while(!(TWCR & (1u<<TWINT)));
    return TWDR;
}

// ---- Public API Implementation ----------------------------------------------------------

void I2C_voidInit(void) {
    // Configure I2C pins (PC0 = SDA, PC1 = SCL)
    // For ATmega32, I2C uses internal hardware, so pins should be set as OUTPUT

    DIO_voidSetPinDirection(DIO_u8_PORTC, DIO_u8_PIN0, DIO_u8_OUTPUT);
    DIO_voidSetPinDirection(DIO_u8_PORTC, DIO_u8_PIN1, DIO_u8_OUTPUT);

    
    // Set bit rate for 100kHz with 8MHz CPU: TWBR = ((F_CPU/SCL)-16)/(2*Prescaler)
    TWBR = 32;
    TWSR = 0x00; // Prescaler = 1
    
    // Enable I2C
    TWCR = (1 << TWEN);
    
    _delay_us(10);
}

u8 I2C_u8Start(void) {
    // Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    // Wait for TWINT flag
    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 1; // Timeout error
    
    // Check status
    u8 status = TWSR & TWSR_STATUS_MASK;
    return (status == 0x08 || status == 0x10) ? 0 : 1; // START or REPEATED START
}

void I2C_voidStop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    // No need to wait for STOP condition to complete
}

u8 I2C_u8GetStatus(void) {
    return TWSR & TWSR_STATUS_MASK;
}

// Low-level byte operations
u8 TWI_WriteByteRaw(u8 u8Data) {
    TWDR = u8Data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 1; // Timeout error

    u8 status = TWSR & TWSR_STATUS_MASK;
    /* Accept SLA+W ACK (0x18), DATA ACK (0x28), or SLA+R ACK (0x40) as success */
    return (status == 0x18 || status == 0x28 || status == 0x40) ? 0 : 1;
}

u8 TWI_ReadByteRaw(u8 ack, u8 *out) {
    if (!out) return 1;
    if (ack) TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    else     TWCR = (1 << TWINT) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 2; // timeout

    *out = TWDR;
    return 0;
}

// High-level device communication functions
u8 I2C_u8WriteReg(u8 addr7, u8 reg, u8 val) {
    if (hlp_start(SLA(addr7, 0))) return 1;
    if (hlp_write(reg)) return 1;
    if (hlp_write(val)) return 1;
    hlp_stop();
    return 0;
}

u8 I2C_u8ReadRegs(u8 addr7, u8 reg, u8* buf, u8 len) {
    if (!buf || len == 0) return 1;

    // START + SLA+W
    if (hlp_start(SLA(addr7, 0))) return 1;
    if (hlp_write(reg))           return 1;

    // REPEATED START + SLA+R
    if (hlp_start(SLA(addr7, 1))) return 1;

    for (u8 i = 0; i < len; i++) {
        buf[i] = (i < (len - 1)) ? hlp_readAck() : hlp_readNack();
    }

    hlp_stop();
    return 0;
}

/**
 * Write a single byte directly to an I2C device (no register address).
 * Used for devices like TCA9548A that don't use register-based communication.
 * Returns 0 on success, 1 on error.
 */
u8 I2C_u8WriteByte(u8 addr7, u8 val) {
    if (hlp_start(SLA(addr7, 0))) return 1;
    if (hlp_write(val)) return 1;
    hlp_stop();
    return 0;
}

/**
 * Read a single byte directly from an I2C device (no register address).
 * Used for devices like TCA9548A that don't use register-based communication.
 * Returns 0 on success, 1 on error.
 */
u8 I2C_u8ReadByte(u8 addr7, u8* val) {
    if (!val) return 1;
    
    // START + SLA+R
    if (hlp_start(SLA(addr7, 1))) return 1;
    
    // Read single byte with NACK
    *val = hlp_readNack();
    
    hlp_stop();
    return 0;
}

// Direct address communication (for simple devices)
u8 I2C_u8WriteByteAddr(u8 addr7, u8 val) {
    if (I2C_u8Start()) return 1;
    /* SLA+W: send address<<1 with write bit (0) */
    if (TWI_WriteByteRaw((addr7 << 1) | 0x00)) {
        I2C_voidStop();
        return 1;
    }
    /* send data */
    if (TWI_WriteByteRaw(val)) {
        I2C_voidStop();
        return 1;
    }
    I2C_voidStop();
    return 0;
}

u8 I2C_u8ReadByteAddr(u8 addr7, u8 *val) {
    if (!val) return 1;
    if (I2C_u8Start()) return 2;
    if (TWI_WriteByteRaw((addr7 << 1) | 0x01)) { I2C_voidStop(); return 3; }
    if (TWI_ReadByteRaw(0, val)) { I2C_voidStop(); return 4; }
    I2C_voidStop();
    return 0;
}

// Helper initialization function
void I2C_helpers_init(u32 i2c_hz) {
    // Prescaler = 1
    TWSR &= ~((1u<<TWPS1) | (1u<<TWPS0));

    // TWBR from formula: SCL = F_CPU / (16 + 2*TWBR*prescale)
    // -> TWBR = (F_CPU / SCL - 16) / (2*prescale)
    // Clamp to 8-bit range
    u32 twbr = 0;
    if (i2c_hz > 0) {
        u32 tmp = (F_CPU / i2c_hz);
        if (tmp > 16) {
            tmp -= 16;
            twbr = tmp / 2;
            if (twbr > 255) twbr = 255;
        }
    }
    TWBR = (u8)twbr;

    // Enable TWI
    TWCR = (1u<<TWEN);
}

// === Compatibility functions (legacy support) ===

void I2C_voidStart(void) {
    (void)I2C_u8Start(); // Use new function but ignore return
}

void I2C_voidWrite(u8 u8Data) {
    (void)TWI_WriteByteRaw(u8Data); // Use raw function
}

u8 I2C_u8ReadWithACK(void) {
    u8 data;
    TWI_ReadByteRaw(1, &data); // Read with ACK
    return data;
}

u8 I2C_u8ReadWithNACK(void) {
    u8 data;
    TWI_ReadByteRaw(0, &data); // Read with NACK
    return data;
}