#include "../src/LIB/STD_TYPES.h"
#include "../src/LIB/BIT_MATH.h"
#include "../src/MCAL/DIO/DIO_Interface.h"
#include "../src/HAL/LCD/LCD_Interface.h"
#include "../src/MCAL/I2C/i2c_helpers.h"
#include "../src/HAL/TCA9548A/TCA9548A_Interface.h"
#include "../src/HAL/TCA9548A/TCA9548A_Integration.h"
#include "../src/MCAL/UART/UART_Interface.h"
#include <util/delay.h>

/*
 * TCA9548A Multiplexer Test Program
 * 
 * This program tests the TCA9548A I2C multiplexer functionality:
 * 1. Initializes all peripherals
 * 2. Tests TCA9548A communication
 * 3. Scans all channels for MPU6050 sensors
 * 4. Displays results on LCD and UART
 */

int main(void) {
    TCA9548A_Status_t mux_status;
    u8 sensors_found = 0;
    
    // Initialize peripherals
    DIO_voidInit();
    UART_voidInit();
    LCD_vidInit_4bits();
    
    // Show initialization progress
    LCD_vidWriteString((u8*)"TCA9548A Test");
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Initializing...");
    
    UART_voidSendString("=================================\r\n");
    UART_voidSendString("TCA9548A I2C Multiplexer Test\r\n");
    UART_voidSendString("=================================\r\n");
    
    _delay_ms(1000);
    
    // Initialize I2C
    UART_voidSendString("Initializing I2C bus...\r\n");
    I2C_helpers_init(100000); // 100 kHz
    _delay_ms(500);
    
    // Initialize TCA9548A
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Init TCA9548A...");
    UART_voidSendString("Initializing TCA9548A multiplexer...\r\n");
    
    mux_status = TCA9548A_Init();
    
    if (mux_status == TCA9548A_OK) {
        LCD_vidGotoxy(0, 1);
        LCD_vidWriteString((u8*)"TCA9548A: OK   ");
        UART_voidSendString("TCA9548A initialization successful!\r\n");
        _delay_ms(1000);
        
        // Test communication
        UART_voidSendString("Testing TCA9548A communication...\r\n");
        mux_status = TCA9548A_TestCommunication();
        
        if (mux_status == TCA9548A_OK) {
            UART_voidSendString("TCA9548A communication test passed!\r\n");
            
            // Test all channels for sensors
            LCD_Clear();
            LCD_vidWriteString((u8*)"Scanning sensors");
            LCD_vidGotoxy(0, 1);
            LCD_vidWriteString((u8*)"Please wait...");
            
            sensors_found = TCA9548A_TestAllSensors();
            
            // Display results
            LCD_Clear();
            LCD_vidWriteString((u8*)"Sensors found:");
            LCD_vidGotoxy(0, 1);
            LCD_vidWriteNumber(sensors_found);
            LCD_vidWriteString((u8*)" of 5");
            
            if (sensors_found > 0) {
                _delay_ms(2000);
                
                // Initialize the complete smart glove system
                LCD_Clear();
                LCD_vidWriteString((u8*)"Init SmartGlove");
                LCD_vidGotoxy(0, 1);
                LCD_vidWriteString((u8*)"System...");
                
                mux_status = TCA9548A_InitializeSmartGloveSystem();
                
                if (mux_status == TCA9548A_OK) {
                    LCD_vidGotoxy(0, 1);
                    LCD_vidWriteString((u8*)"System: Ready!");
                    UART_voidSendString("Smart Glove System initialized successfully!\r\n");
                } else {
                    LCD_vidGotoxy(0, 1);
                    LCD_vidWriteString((u8*)"System: Error!");
                    UART_voidSendString("Smart Glove System initialization failed!\r\n");
                }
            }
        } else {
            LCD_vidGotoxy(0, 1);
            LCD_vidWriteString((u8*)"Comm: FAILED   ");
            UART_voidSendString("TCA9548A communication test failed!\r\n");
        }
    } else {
        LCD_vidGotoxy(0, 1);
        LCD_vidWriteString((u8*)"TCA9548A: ERROR");
        UART_voidSendString("TCA9548A initialization failed! Status: ");
        UART_voidSendString((u8*)TCA9548A_GetStatusString(mux_status));
        UART_voidSendString("\r\n");
    }
    
    _delay_ms(3000);
    
    // Main loop - demonstrate channel switching
    u8 current_channel = 0;
    
    while(1) {
        if (mux_status == TCA9548A_OK && sensors_found > 0) {
            // Cycle through available channels
            LCD_Clear();
            LCD_vidWriteString((u8*)"Active Channel:");
            LCD_vidGotoxy(0, 1);
            LCD_vidWriteNumber(current_channel);
            
            // Select channel
            TCA9548A_SelectChannel(current_channel);
            
            // Show which finger this represents
            LCD_vidWriteString((u8*)" - ");
            switch(current_channel) {
                case 0: LCD_vidWriteString((u8*)"Thumb"); break;
                case 1: LCD_vidWriteString((u8*)"Index"); break;
                case 2: LCD_vidWriteString((u8*)"Middle"); break;
                case 3: LCD_vidWriteString((u8*)"Ring"); break;
                case 4: LCD_vidWriteString((u8*)"Palm"); break;
                default: LCD_vidWriteString((u8*)"N/A"); break;
            }
            
            // Send channel info via UART
            UART_voidSendString("Selected Channel ");
            UART_voidSendNumber(current_channel);
            UART_voidSendString("\r\n");
            
            // Move to next channel
            current_channel++;
            if (current_channel >= 5) {
                current_channel = 0;
            }
            
        } else {
            // Error state - just blink
            LCD_Clear();
            LCD_vidWriteString((u8*)"System Error");
            LCD_vidGotoxy(0, 1);
            LCD_vidWriteString((u8*)"Check Hardware");
            _delay_ms(500);
            
            LCD_Clear();
            _delay_ms(500);
        }
        
        _delay_ms(2000);
    }
    
    return 0;
}
