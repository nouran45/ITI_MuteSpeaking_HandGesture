/*
 * TCA9548A_Diagnostic.c
 * 
 * Diagnostic functions to test TCA9548A multiplexer communication
 */

#include "STD_TYPES.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Config.h"
#include "../../MCAL/I2C/i2c_helpers.h"
#include "../../MCAL/UART/UART_Interface.h"
#include <util/delay.h>

/**
 * @brief Test basic TCA9548A communication by reading back channel selection
 */
void TCA9548A_DiagnosticTest(void) {
    UART_voidSendString("\r\n=== TCA9548A Diagnostic Test ===\r\n");
    
    // Test 1: Check if TCA9548A responds to I2C address
    UART_voidSendString("Test 1: TCA9548A I2C Address Detection\r\n");
    
    // Try to read current channel selection (should return current state)
    u8 current_state;
    if (I2C_u8ReadByte(TCA9548A_I2C_ADDRESS, &current_state) == 0) {
        UART_voidSendString("  TCA9548A detected! Current state: 0x");
        if (current_state >= 0x10) {
            UART_voidSendNumber(current_state >> 4);
        } else {
            UART_voidSendString("0");
        }
        UART_voidSendNumber(current_state & 0x0F);
        UART_voidSendString("\r\n");
    } else {
        UART_voidSendString("  ERROR: TCA9548A not responding at address 0x70!\r\n");
        UART_voidSendString("  Check wiring and I2C connections.\r\n");
        return;
    }
    
    // Test 2: Test channel selection and readback
    UART_voidSendString("\r\nTest 2: Channel Selection Test\r\n");
    
    for (u8 channel = 0; channel < 5; channel++) {
        UART_voidSendString("  Testing channel ");
        UART_voidSendNumber(channel);
        UART_voidSendString("...\r\n");
        
        // Calculate channel mask (1 << channel)
        u8 channel_mask = (1 << channel);
        
        // Write channel selection
        if (I2C_u8WriteByte(TCA9548A_I2C_ADDRESS, channel_mask) == 0) {
            UART_voidSendString("    Write OK: 0x");
            if (channel_mask >= 0x10) {
                UART_voidSendNumber(channel_mask >> 4);
            } else {
                UART_voidSendString("0");
            }
            UART_voidSendNumber(channel_mask & 0x0F);
            
            // Small delay
            _delay_ms(5);
            
            // Read back to verify
            u8 readback;
            if (I2C_u8ReadByte(TCA9548A_I2C_ADDRESS, &readback) == 0) {
                UART_voidSendString(", Read: 0x");
                if (readback >= 0x10) {
                    UART_voidSendNumber(readback >> 4);
                } else {
                    UART_voidSendString("0");
                }
                UART_voidSendNumber(readback & 0x0F);
                
                if (readback == channel_mask) {
                    UART_voidSendString(" ✓ PASS\r\n");
                } else {
                    UART_voidSendString(" ✗ MISMATCH!\r\n");
                }
            } else {
                UART_voidSendString(", Read FAILED!\r\n");
            }
        } else {
            UART_voidSendString("    Write FAILED!\r\n");
        }
        
        _delay_ms(100);
    }
    
    // Test 3: Test MPU6050 detection on each channel
    UART_voidSendString("\r\nTest 3: MPU6050 Detection per Channel\r\n");
    
    for (u8 channel = 0; channel < 5; channel++) {
        UART_voidSendString("  Channel ");
        UART_voidSendNumber(channel);
        UART_voidSendString(": ");
        
        // Select channel
        u8 channel_mask = (1 << channel);
        if (I2C_u8WriteByte(TCA9548A_I2C_ADDRESS, channel_mask) == 0) {
            _delay_ms(10);  // Allow channel to settle
            
            // Try to read WHO_AM_I from MPU6050
            u8 who_am_i;
            if (I2C_u8ReadRegs(0x68, 0x75, &who_am_i, 1) == 0) {
                if (who_am_i == 0x68) {
                    UART_voidSendString("MPU6050 FOUND (WHO_AM_I=0x68) ✓\r\n");
                } else {
                    UART_voidSendString("Device found but WHO_AM_I=0x");
                    if (who_am_i >= 0x10) {
                        UART_voidSendNumber(who_am_i >> 4);
                    } else {
                        UART_voidSendString("0");
                    }
                    UART_voidSendNumber(who_am_i & 0x0F);
                    UART_voidSendString(" (not MPU6050)\r\n");
                }
            } else {
                UART_voidSendString("No I2C device found\r\n");
            }
        } else {
            UART_voidSendString("Channel select FAILED\r\n");
        }
        
        _delay_ms(50);
    }
    
    // Test 4: Disable all channels
    UART_voidSendString("\r\nTest 4: Disable All Channels\r\n");
    if (I2C_u8WriteByte(TCA9548A_I2C_ADDRESS, 0x00) == 0) {
        _delay_ms(5);
        u8 readback;
        if (I2C_u8ReadByte(TCA9548A_I2C_ADDRESS, &readback) == 0) {
            if (readback == 0x00) {
                UART_voidSendString("  All channels disabled ✓\r\n");
            } else {
                UART_voidSendString("  ERROR: Expected 0x00, got 0x");
                if (readback >= 0x10) {
                    UART_voidSendNumber(readback >> 4);
                } else {
                    UART_voidSendString("0");
                }
                UART_voidSendNumber(readback & 0x0F);
                UART_voidSendString("\r\n");
            }
        } else {
            UART_voidSendString("  Readback failed\r\n");
        }
    } else {
        UART_voidSendString("  Write failed\r\n");
    }
    
    UART_voidSendString("\r\n=== Diagnostic Test Complete ===\r\n");
}

/**
 * @brief Quick test to scan for I2C devices
 */
void TCA9548A_I2CScanTest(void) {
    UART_voidSendString("\r\n=== I2C Device Scan ===\r\n");
    UART_voidSendString("Scanning I2C addresses 0x08-0x77...\r\n");
    
    u8 devices_found = 0;
    
    for (u8 addr = 0x08; addr <= 0x77; addr++) {
        // Try to detect device by attempting to read
        u8 dummy;
        if (I2C_u8ReadByte(addr, &dummy) == 0) {
            UART_voidSendString("Device found at 0x");
            if (addr >= 0x10) {
                UART_voidSendNumber(addr >> 4);
            } else {
                UART_voidSendString("0");
            }
            UART_voidSendNumber(addr & 0x0F);
            
            // Identify common devices
            if (addr == 0x68) {
                UART_voidSendString(" (MPU6050)");
            } else if (addr == 0x70) {
                UART_voidSendString(" (TCA9548A)");
            }
            UART_voidSendString("\r\n");
            devices_found++;
        }
        _delay_ms(2);  // Small delay between scans
    }
    
    UART_voidSendString("I2C scan complete. Found ");
    UART_voidSendNumber(devices_found);
    UART_voidSendString(" devices.\r\n");
}
