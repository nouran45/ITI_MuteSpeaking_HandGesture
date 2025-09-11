/*
 * TCA9548A_Diagnostic.h
 * 
 * Header for TCA9548A diagnostic functions
 */

#ifndef TCA9548A_DIAGNOSTIC_H
#define TCA9548A_DIAGNOSTIC_H

#include "STD_TYPES.h"

/**
 * @brief Comprehensive diagnostic test for TCA9548A multiplexer
 * Tests I2C communication, channel selection, and MPU6050 detection
 */
void TCA9548A_DiagnosticTest(void);

/**
 * @brief Scan all I2C addresses to detect devices
 * Useful for troubleshooting I2C bus issues
 */
void TCA9548A_I2CScanTest(void);

#endif /* TCA9548A_DIAGNOSTIC_H */
