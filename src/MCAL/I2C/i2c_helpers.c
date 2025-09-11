/*
 * i2c_helpers.c
 *
 *  Created on: Sep 4, 2025
 *      Author: yasser
 */


#include <avr/io.h>
#include "STD_TYPES.h"
#include "i2c_helpers.h"

// ---- Internal utilities -------------------------------------------------

static inline u8 twi_status(void) { return (TWSR & 0xF8); }

// SLA+R/W byte builder
static inline u8 SLA(u8 addr7, u8 rw) { return (u8)((addr7 << 1) | (rw & 1)); }

// START or REPEATED START + SLA+R/W
static u8 hlp_start(u8 addr_rw)
{
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

static inline void hlp_stop(void)
{
    TWCR = (1u<<TWINT) | (1u<<TWEN) | (1u<<TWSTO);
    // No wait required; bus releases automatically
}

static u8 hlp_write(u8 data)
{
    TWDR = data;
    TWCR = (1u<<TWINT) | (1u<<TWEN);
    while(!(TWCR & (1u<<TWINT)));
    // Data transmitted, ACK received => 0x28
    return (twi_status() == 0x28) ? 0 : 1;
}

static u8 hlp_readAck(void)
{
    TWCR = (1u<<TWINT) | (1u<<TWEN) | (1u<<TWEA);
    while(!(TWCR & (1u<<TWINT)));
    return TWDR;
}

static u8 hlp_readNack(void)
{
    TWCR = (1u<<TWINT) | (1u<<TWEN);
    while(!(TWCR & (1u<<TWINT)));
    return TWDR;
}

// ---- Public API ----------------------------------------------------------

void I2C_helpers_init(u32 i2c_hz)
{
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

u8 I2C_u8ReadRegs(u8 addr7, u8 reg, u8* buf, u8 len)
{
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

u8 I2C_u8WriteReg(u8 addr7, u8 reg, u8 val)
{
    if (hlp_start(SLA(addr7, 0))) return 1;
    if (hlp_write(reg)) return 1;
    if (hlp_write(val)) return 1;
    hlp_stop();
    return 0;
}

/**
 * Write a single byte directly to an I2C device (no register address).
 * Used for devices like TCA9548A that don't use register-based communication.
 * Returns 0 on success, 1 on error.
 */
u8 I2C_u8WriteByte(u8 addr7, u8 val)
{
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
u8 I2C_u8ReadByte(u8 addr7, u8* val)
{
    if (!val) return 1;
    
    // START + SLA+R
    if (hlp_start(SLA(addr7, 1))) return 1;
    
    // Read single byte with NACK
    *val = hlp_readNack();
    
    hlp_stop();
    return 0;
}
