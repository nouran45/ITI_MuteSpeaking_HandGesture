#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "../MCAL/DIO/DIO_Interface.h"
#include "../HAL/LCD/LCD_Interface.h"
#include "../MCAL/I2C/I2C_Interface.h"
#include "../HAL/MPU6050/MPU6050_Interface.h"
#include "../MCAL/UART/UART_Interface.h"
#include "../MCAL/DIO/DIO_Register.h"
#include <util/delay.h>
#include <stdio.h>  // For sprintf function

int main(void) {
    s16 s16AccelX, s16AccelY, s16AccelZ;
    s16 s16GyroX, s16GyroY, s16GyroZ;
    
    // Initialize peripherals
    DIO_voidInit();
    UART_voidInit();  // Initialize UART for laptop communication
    LCD_vidInit_4bits();  // Using 4-bit mode to free up PC0 for I2C
    
    // Show initialization progress
    LCD_vidWriteString((u8*)"LCD: OK");
    _delay_ms(1000);
    
    // Send initialization message to laptop
    UART_voidSendString("MPU6050 Sensor Data Stream Starting...\r\n");
    UART_voidSendString("Format: AX,AY,AZ,GX,GY,GZ,Motion\r\n");
    UART_voidSendString("--------------------------------------\r\n");
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"I2C Init...");
    I2C_voidInit();  // Try the improved I2C init with timeouts
    _delay_ms(500);
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"I2C: OK     ");
    _delay_ms(500);
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"MPU Init...");
    MPU6050_voidInit();
    _delay_ms(1000);  // Give MPU6050 more time to initialize
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"MPU: OK     ");
    _delay_ms(500);
    
    // Simple test - just show we're ready
    LCD_Clear();
    LCD_vidWriteString((u8*)"Streaming to PC");
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Check Serial...");
    _delay_ms(2000);  // Give even more time
    
    while(1) {
        LCD_Clear();
        
        // Read sensor data
        s16AccelX = 0, s16AccelY = 0, s16AccelZ = 0;
        s16GyroX = 0, s16GyroY = 0, s16GyroZ = 0;
        
        // Read accelerometer and gyroscope data
        MPU6050_voidReadAccel(&s16AccelX, &s16AccelY, &s16AccelZ);
        MPU6050_voidReadGyro(&s16GyroX, &s16GyroY, &s16GyroZ);
        
        // Send live data to laptop in CSV format
        UART_voidSendNumber(s16AccelX);
        UART_voidSendString(",");
        UART_voidSendNumber(s16AccelY);
        UART_voidSendString(",");
        UART_voidSendNumber(s16AccelZ);
        UART_voidSendString(",");
        UART_voidSendNumber(s16GyroX);
        UART_voidSendString(",");
        UART_voidSendNumber(s16GyroY);
        UART_voidSendString(",");
        UART_voidSendNumber(s16GyroZ);
        UART_voidSendString(",");
        
        // Send motion detection status
        if (s16AccelX > 5000) {
            UART_voidSendString("TILT_RIGHT");
        } else if (s16AccelX < -5000) {
            UART_voidSendString("TILT_LEFT");
        } else if (s16AccelY > 5000) {
            UART_voidSendString("TILT_BACK");
        } else if (s16AccelY < -5000) {
            UART_voidSendString("TILT_FORWARD");
        } else if (s16AccelZ > 12000) {
            UART_voidSendString("FACE_UP");
        } else if (s16AccelZ < -12000) {
            UART_voidSendString("FACE_DOWN");
        } else {
            UART_voidSendString("LEVEL");
        }
        
        // Add rotation info
        if (s16GyroZ > 3000) {
            UART_voidSendString("_SPIN_RIGHT");
        } else if (s16GyroZ < -3000) {
            UART_voidSendString("_SPIN_LEFT");
        }
        
        UART_voidSendString("\r\n");  // End of line for CSV
        
        // Display simplified info on LCD
        LCD_vidGotoxy(0, 0);
        LCD_vidWriteString((u8*)"AX:");
        LCD_vidWriteSignedNumber(s16AccelX / 1000);
        LCD_vidWriteString((u8*)" Y:");
        LCD_vidWriteSignedNumber(s16AccelY / 1000);
        
        LCD_vidGotoxy(0, 1);
        LCD_vidWriteString((u8*)"AZ:");
        LCD_vidWriteSignedNumber(s16AccelZ / 1000);
        LCD_vidWriteString((u8*)" GZ:");
        LCD_vidWriteSignedNumber(s16GyroZ / 1000);
        
        _delay_ms(200);  // Faster updates for live streaming
    }
    
    return 0;
}