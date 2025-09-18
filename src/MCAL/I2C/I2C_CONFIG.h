#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

// I2C Status Codes
#define I2C_START         0x08
#define I2C_REP_START     0x10
#define I2C_MT_SLA_ACK    0x18
#define I2C_MT_SLA_NACK   0x20
#define I2C_MT_DATA_ACK   0x28
#define I2C_MT_DATA_NACK  0x30
#define I2C_MR_SLA_ACK    0x40
#define I2C_MR_SLA_NACK   0x48
#define I2C_MR_DATA_ACK   0x50
#define I2C_MR_DATA_NACK  0x58


#define I2C_PRESCALER_1   0
#define I2C_PRESCALER_4   1
#define I2C_PRESCALER_16  2
#define I2C_PRESCALER_64  3

// I2C Bit Rate
#define I2C_BIT_RATE      0x47  // 100kHz at 8MHz CPU clock

#endif
