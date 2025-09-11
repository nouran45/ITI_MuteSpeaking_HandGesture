/*
 * TCA9548A Simple Test Program
 * 
 * A minimal test to diagnose TCA9548A communication issues
 */

#include "STD_TYPES.h"
#include "LCD_Interface.h"
#include "MPU6050_Interface.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Diagnostic.h"
#include "UART_Interface.h"
#include "I2C_Interface.h"
#include "DIO_Interface.h"
#include "i2c_helpers.h"
#include <util/delay.h>

int main(void) {
    // Initialize DIO
    DIO_voidInit();
    
    // Initialize LCD
    LCD_Init();
    LCD_Clear();
    LCD_vidWriteString((u8*)"TCA9548A Test");
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Starting...");
    
    // Initialize UART for debug output
    UART_voidInit();
    _delay_ms(100);
    
    // Initialize I2C
    I2C_voidInit();
    _delay_ms(500);
    
    UART_voidSendString("\r\n\r\n=== TCA9548A Communication Test ===\r\n");
    UART_voidSendString("Version: Simple Direct Test\r\n");
    UART_voidSendString("MCU: ATmega32 @ 8MHz\r\n");
    UART_voidSendString("I2C: TWI Hardware\r\n\r\n");
    
    LCD_Clear();
    LCD_vidWriteString((u8*)"I2C Scan...");
    
    // First, scan for I2C devices
    TCA9548A_I2CScanTest();
    
    LCD_Clear();
    LCD_vidWriteString((u8*)"TCA Test...");
    
    // Then run comprehensive TCA9548A test
    TCA9548A_DiagnosticTest();
    
    LCD_Clear();
    LCD_vidWriteString((u8*)"Test Complete");
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Check UART");
    
    UART_voidSendString("\r\n=== All Tests Complete ===\r\n");
    UART_voidSendString("Review results above to diagnose TCA9548A issues.\r\n");
    
    // Simple continuous test loop
    u8 test_counter = 0;
    
    while(1) {
        _delay_ms(5000);  // 5 second delay
        
        test_counter++;
        UART_voidSendString("\r\n--- Quick Test #");
        UART_voidSendNumber(test_counter);
        UART_voidSendString(" ---\r\n");
        
        // Test TCA9548A basic communication
        u8 state;
        if (I2C_u8ReadByte(0x70, &state) == 0) {
            UART_voidSendString("TCA9548A responding, state: 0x");
            if (state >= 0x10) {
                UART_voidSendNumber(state >> 4);
            } else {
                UART_voidSendString("0");
            }
            UART_voidSendNumber(state & 0x0F);
            UART_voidSendString("\r\n");
            
            // Try to select channel 0 and read MPU6050
            if (I2C_u8WriteByte(0x70, 0x01) == 0) {  // Select channel 0
                _delay_ms(10);
                u8 who_am_i;
                if (I2C_u8ReadRegs(0x68, 0x75, &who_am_i, 1) == 0) {
                    UART_voidSendString("Channel 0 MPU6050 WHO_AM_I: 0x");
                    if (who_am_i >= 0x10) {
                        UART_voidSendNumber(who_am_i >> 4);
                    } else {
                        UART_voidSendString("0");
                    }
                    UART_voidSendNumber(who_am_i & 0x0F);
                    UART_voidSendString("\r\n");
                } else {
                    UART_voidSendString("Channel 0 MPU6050 not responding\r\n");
                }
            } else {
                UART_voidSendString("Channel 0 select failed\r\n");
            }
        } else {
            UART_voidSendString("TCA9548A not responding!\r\n");
        }
        
        // Update LCD with test counter
        LCD_Clear();
        LCD_vidWriteString((u8*)"Test #");
        LCD_vidWriteNumber(test_counter);
        LCD_vidGotoxy(0, 1);
        LCD_vidWriteString((u8*)"Check UART");
    }
    
    return 0;
}
