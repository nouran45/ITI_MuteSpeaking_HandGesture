#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "I2C_Interface.h"
#include "I2C_CONFIG.h"
#include "DIO_Interface.h"
#include "DIO_Register.h"
#include <util/delay.h>

// I2C Timeout
#define I2C_TIMEOUT 10000

// Helper functions
static inline u8 twi_status(void) {
    return (TWSR & TWSR_STATUS_MASK);
}

static inline u8 SLA(u8 addr7, u8 rw) {
    return (u8)((addr7 << 1) | (rw & 1));
}

static u8 hlp_start(u8 addr_rw) {
    // Send START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 1;

    u8 st = twi_status();
    if (st != 0x08 && st != 0x10) return 1;

    // Send SLA+R/W
    TWDR = addr_rw;
    TWCR = (1 << TWINT) | (1 << TWEN);

    timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 1;

    st = twi_status();
    if (st != 0x18 && st != 0x40) return 1;

    return 0;
}

static inline void hlp_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    _delay_us(10);
}

static u8 hlp_write(u8 data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 1;

    return (twi_status() == 0x28) ? 0 : 1;
}

static u8 hlp_readAck(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 0;

    return TWDR;
}

static u8 hlp_readNack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 0;

    return TWDR;
}

// Public API Implementation
void I2C_voidInit(void) {
    // Configure I2C pins as output
    DIO_voidSetPinDirection(DIO_u8_PORTC, DIO_u8_PIN0, DIO_u8_OUTPUT);
    DIO_voidSetPinDirection(DIO_u8_PORTC, DIO_u8_PIN1, DIO_u8_OUTPUT);

    // Set bit rate for 100kHz with 8MHz CPU
    TWBR = 32;
    TWSR = 0x00;

    // Enable I2C
    TWCR = (1 << TWEN);
    _delay_us(10);
}

u8 I2C_u8Start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 1;

    u8 status = twi_status();
    return (status == 0x08 || status == 0x10) ? 0 : 1;
}

void I2C_voidStop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(10);
}

u8 I2C_u8GetStatus(void) {
    return twi_status();
}

u8 TWI_WriteByteRaw(u8 u8Data) {
    TWDR = u8Data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 1;

    u8 status = twi_status();
    return (status == 0x18 || status == 0x28 || status == 0x40) ? 0 : 1;
}

u8 TWI_ReadByteRaw(u8 ack, u8 *out) {
    if (!out) return 1;

    if (ack)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    else
        TWCR = (1 << TWINT) | (1 << TWEN);

    u16 timeout = I2C_TIMEOUT;
    while (!(TWCR & (1 << TWINT)) && timeout--);
    if (!timeout) return 2;

    *out = TWDR;
    return 0;
}

u8 I2C_u8WriteReg(u8 addr7, u8 reg, u8 val) {
    if (hlp_start(SLA(addr7, 0))) return 1;
    if (hlp_write(reg)) return 1;
    if (hlp_write(val)) return 1;
    hlp_stop();
    return 0;
}

u8 I2C_u8ReadRegs(u8 addr7, u8 reg, u8* buf, u8 len) {
    if (!buf || len == 0) return 1;

    if (hlp_start(SLA(addr7, 0))) return 1;
    if (hlp_write(reg)) return 1;
    if (hlp_start(SLA(addr7, 1))) return 1;

    for (u8 i = 0; i < len; i++) {
        buf[i] = (i < (len - 1)) ? hlp_readAck() : hlp_readNack();
    }

    hlp_stop();
    return 0;
}

u8 I2C_u8WriteByte(u8 addr7, u8 val) {
    if (hlp_start(SLA(addr7, 0))) return 1;
    if (hlp_write(val)) return 1;
    hlp_stop();
    return 0;
}

u8 I2C_u8ReadByte(u8 addr7, u8* val) {
    if (!val) return 1;

    if (hlp_start(SLA(addr7, 1))) return 1;
    *val = hlp_readNack();
    hlp_stop();
    return 0;
}

u8 I2C_u8WriteByteAddr(u8 addr7, u8 val) {
    if (I2C_u8Start()) return 1;
    if (TWI_WriteByteRaw((addr7 << 1) | 0x00)) {
        I2C_voidStop();
        return 1;
    }
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
    if (TWI_WriteByteRaw((addr7 << 1) | 0x01)) {
        I2C_voidStop();
        return 3;
    }
    if (TWI_ReadByteRaw(0, val)) {
        I2C_voidStop();
        return 4;
    }
    I2C_voidStop();
    return 0;
}

void I2C_helpers_init(u32 i2c_hz) {
    TWSR = 0x00;
    u32 twbr = (F_CPU / i2c_hz - 16) / 2;
    if (twbr > 255) twbr = 255;
    TWBR = (u8)twbr;
    TWCR = (1 << TWEN);
}

// Compatibility functions
void I2C_voidStart(void) {
    (void)I2C_u8Start();
}

void I2C_voidWrite(u8 u8Data) {
    (void)TWI_WriteByteRaw(u8Data);
}

u8 I2C_u8ReadWithACK(void) {
    u8 data;
    TWI_ReadByteRaw(1, &data);
    return data;
}

u8 I2C_u8ReadWithNACK(void) {
    u8 data;
    TWI_ReadByteRaw(0, &data);
    return data;
}
