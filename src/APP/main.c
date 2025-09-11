#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "LCD_Interface.h"
#include "I2C_Interface.h"
#include "MPU6050_Interface.h"
#include "UART_Interface.h"
#include "DIO_Register.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Integration.h"
#include "mpu6050_types.h"
#include "i2c_helpers.h"
#include <util/delay.h>
#include <stdio.h>

// Define your 5 sensor configurations
mpu6050_dev_t sensor_configs[5] = {
    // Thumb sensor (uses mux channel 0)
    { .i2c_addr = 0x68, .mux_channel = 0, .ax_off=0, .ay_off=0, .az_off=0, .gx_off=0, .gy_off=0, .gz_off=0 },
    // Index finger sensor (uses mux channel 1)  
    { .i2c_addr = 0x68, .mux_channel = 1, .ax_off=0, .ay_off=0, .az_off=0, .gx_off=0, .gy_off=0, .gz_off=0 },
    // Middle finger sensor (uses mux channel 2)
    { .i2c_addr = 0x68, .mux_channel = 2, .ax_off=0, .ay_off=0, .az_off=0, .gx_off=0, .gy_off=0, .gz_off=0 },
    // Ring finger sensor (uses mux channel 3)
    { .i2c_addr = 0x68, .mux_channel = 3, .ax_off=0, .ay_off=0, .az_off=0, .gx_off=0, .gy_off=0, .gz_off=0 },
    // Palm sensor (uses mux channel 4)
    { .i2c_addr = 0x68, .mux_channel = 4, .ax_off=0, .ay_off=0, .az_off=0, .gx_off=0, .gy_off=0, .gz_off=0 }
};

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
    UART_voidSendString("Smart Glove System Starting...\r\n");
    UART_voidSendString("5-Sensor MPU6050 with TCA9548A Multiplexer\r\n");
    UART_voidSendString("-------------------------------------------\r\n");
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"I2C Init...");
    I2C_voidInit();  // Try the improved I2C init with timeouts
    _delay_ms(500);
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"I2C: OK     ");
    _delay_ms(500);

    // Initialize TCA9548A multiplexer
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"MUX Init...");
    TCA9548A_Status_t mux_status = TCA9548A_Init();
    _delay_ms(500);
    
    if (mux_status != TCA9548A_OK) {
        LCD_vidGotoxy(0, 1);
        LCD_vidWriteString((u8*)"MUX: FAILED!");
        UART_voidSendString("TCA9548A Init FAILED! Status: ");
        UART_voidSendNumber(mux_status);
        UART_voidSendString("\r\n");
        UART_voidSendString("Attempting recovery...\r\n");
        
        // Attempt recovery 3 times
        for (u8 retry = 0; retry < 3; retry++) {
            _delay_ms(1000);
            LCD_vidGotoxy(0, 1);
            LCD_vidWriteString((u8*)"Retry MUX...");
            mux_status = TCA9548A_Init();
            if (mux_status == TCA9548A_OK) {
                LCD_vidGotoxy(0, 1);
                LCD_vidWriteString((u8*)"MUX: Recovered!");
                UART_voidSendString("TCA9548A Recovery successful!\r\n");
                break;
            }
        }
        
        if (mux_status != TCA9548A_OK) {
            LCD_vidGotoxy(0, 1);
            LCD_vidWriteString((u8*)"MUX: CRITICAL!");
            UART_voidSendString("TCA9548A CRITICAL FAILURE - Cannot continue\r\n");
            while(1) {
                // Flash error indication
                LCD_vidGotoxy(0, 1);
                LCD_vidWriteString((u8*)"SYSTEM HALTED  ");
                _delay_ms(500);
                LCD_vidGotoxy(0, 1);
                LCD_vidWriteString((u8*)"              ");
                _delay_ms(500);
            }
        }
    }
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"MUX: OK     ");
    _delay_ms(500);
    
    // Register all 5 sensors with the MPU6050 driver
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Sensors Reg...");
    for(int i = 0; i < 5; i++) {
        MPU6050_registerSensor(i, &sensor_configs[i]);
    }

    // Initialize the TCA9548A multiplexer callback
    MPU6050_setMuxSelector(TCA9548A_SelectChannelCallback);
    
    // Initialize all MPU6050 sensors
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"MPU Init...");
    MPU6050_initAllSensors();
    _delay_ms(1000);  // Give MPU6050 time to initialize
    
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"MPU: OK     ");
    _delay_ms(500);
    
    // Test all sensors
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Testing Sensors...");
    u8 sensors_found = TCA9548A_TestAllSensors();
    _delay_ms(1000);

    UART_voidSendString("Testing all sensors on TCA9548A channels...\r\n");
    for (u8 channel = 0; channel < 5; channel++) {
        UART_voidSendString("Channel ");
        UART_voidSendNumber(channel);
        
        // First select the channel
        TCA9548A_Status_t select_status = TCA9548A_SelectChannel(channel);
        if (select_status != TCA9548A_OK) {
            UART_voidSendString(": Channel select failed\r\n");
            continue;
        }
        
        u8 who_am_i = 0;
        // Use I2C helpers directly since channel is already selected
        u8 i2c_status = I2C_u8ReadRegs(0x68, 0x75, &who_am_i, 1);
        TCA9548A_Status_t status = (i2c_status == 0) ? TCA9548A_OK : TCA9548A_ERROR_I2C;
        
        if (status == TCA9548A_OK && who_am_i == 0x68) {
            UART_voidSendString(": MPU6050 found (WHO_AM_I=0x");
            if (who_am_i >= 0x10) {
                UART_voidSendNumber(who_am_i >> 4);
            } else {
                UART_voidSendString("0");
            }
            UART_voidSendNumber(who_am_i & 0x0F);
            UART_voidSendString(")\r\n");
        } else {
            UART_voidSendString(": No MPU6050 found or communication error\r\n");
        }
    }
    
    UART_voidSendString("Sensor scan complete. Found ");
    UART_voidSendNumber(sensors_found);
    UART_voidSendString(" sensors.\r\n");
    
    // Disable all channels after testing
    TCA9548A_DisableAllChannels();

    LCD_Clear();
    LCD_vidWriteString((u8*)"Sensors Found:");
    LCD_vidWriteNumber(sensors_found);
    LCD_vidWriteString((u8*)"/5");
    _delay_ms(2000);

    // Simple test - just show we're ready
    LCD_Clear();
    LCD_vidWriteString((u8*)"Streaming to PC");
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Check Serial...");
    _delay_ms(2000);  // Give even more time
    
    while(1) {
        LCD_Clear();
        u8 successful_reads = 0;
        
        // Read sensor data from all 5 sensors
        for(int sensor_id = 0; sensor_id < 5; sensor_id++) {
            // Initialize with error values
            s16AccelX = 0xFFFF; s16AccelY = 0xFFFF; s16AccelZ = 0xFFFF;
            s16GyroX = 0xFFFF; s16GyroY = 0xFFFF; s16GyroZ = 0xFFFF;
            
            // Try to read accelerometer and gyroscope data with error checking
            mpu6050_status_t accel_status = MPU6050_readAll(sensor_id, 
                &s16AccelX, &s16AccelY, &s16AccelZ, NULL, 
                &s16GyroX, &s16GyroY, &s16GyroZ);
            
            if (accel_status == MPU6050_OK) {
                successful_reads++;
                // Send live data to laptop in CSV format
                UART_voidSendString("S");
                UART_voidSendNumber(sensor_id);
                UART_voidSendString(":AX=");
                UART_voidSendNumber(s16AccelX);
                UART_voidSendString(",AY=");
                UART_voidSendNumber(s16AccelY);
                UART_voidSendString(",AZ=");
                UART_voidSendNumber(s16AccelZ);
                UART_voidSendString(",GX=");
                UART_voidSendNumber(s16GyroX);
                UART_voidSendString(",GY=");
                UART_voidSendNumber(s16GyroY);
                UART_voidSendString(",GZ=");
                UART_voidSendNumber(s16GyroZ);
                UART_voidSendString(";");
                
                // Display simplified info on LCD for first sensor only
                if(sensor_id == 0) {
                    LCD_vidGotoxy(0, 0);
                    LCD_vidWriteString((u8*)"S0 AX:");
                    LCD_vidWriteSignedNumber(s16AccelX / 1000);
                    LCD_vidWriteString((u8*)" AY:");
                    LCD_vidWriteSignedNumber(s16AccelY / 1000);
                    
                    LCD_vidGotoxy(0, 1);
                    LCD_vidWriteString((u8*)"AZ:");
                    LCD_vidWriteSignedNumber(s16AccelZ / 1000);
                    LCD_vidWriteString((u8*)" GZ:");
                    LCD_vidWriteSignedNumber(s16GyroZ / 1000);
                }
            } else {
                // Send error indicator for failed sensor
                UART_voidSendString("S");
                UART_voidSendNumber(sensor_id);
                UART_voidSendString(":ERROR;");
                
                // Show error on LCD for first sensor
                if(sensor_id == 0) {
                    LCD_vidGotoxy(0, 0);
                    LCD_vidWriteString((u8*)"S0: READ ERROR");
                    LCD_vidGotoxy(0, 1);
                    LCD_vidWriteString((u8*)"Status: ");
                    LCD_vidWriteNumber(accel_status);
                }
            }
        }
        
        // Add success rate to UART output
        UART_voidSendString("OK:");
        UART_voidSendNumber(successful_reads);
        UART_voidSendString("/5\r\n");  // End of line for CSV
        _delay_ms(200);  // Faster updates for live streaming
    }
    
    return 0;
}