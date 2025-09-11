/*
 * i2c_helpers.h
 *
 *  Created on: Sep 4, 2025
 *      Author: yasser
 */

#ifndef I2C_HELPERS_H
#define I2C_HELPERS_H

#include "STD_TYPES.h"
#include <stdint.h>
/**
 * Initialise AVR TWI (I2C) for a target bus speed (Hz).
 * Example: I2C_helpers_init(100000) for 100 kHz at F_CPU=8 MHz.
 * Safe to call multiple times; enables TWI.
 */
void I2C_helpers_init(u32 i2c_hz);

/**
 * Read multiple consecutive registers from a 7-bit I2C device.
 * addr7: 7-bit address (e.g., 0x68 for MPU6050)
 * reg  : starting register
 * buf  : output buffer
 * len  : number of bytes to read
 * Returns 0 on success, 1 on error.
 */
u8 I2C_u8ReadRegs(u8 addr7, u8 reg, u8* buf, u8 len);

/**
 * Write a single register on a 7-bit I2C device.
 * Returns 0 on success, 1 on error.
 */
u8 I2C_u8WriteReg(u8 addr7, u8 reg, u8 val);

/**
 * Write a single byte directly to an I2C device (no register address).
 * Used for devices like TCA9548A that don't use register-based communication.
 * Returns 0 on success, 1 on error.
 */
u8 I2C_u8WriteByte(u8 addr7, u8 val);

/**
 * Read a single byte directly from an I2C device (no register address).
 * Used for devices like TCA9548A that don't use register-based communication.
 * Returns 0 on success, 1 on error.
 */
u8 I2C_u8ReadByte(u8 addr7, u8* val);

#endif // I2C_HELPERS_H

