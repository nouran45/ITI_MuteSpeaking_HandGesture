#include "STD_TYPES.h"
#include "MPU6050_Interface.h"
#include "I2C_Interface.h" 
#include "TCA9548A_Integration.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Config.h"
#include "UART_Interface.h"
#include <util/delay.h>
#include <MPU6050/MPU6050_Interface.h>

// MPU6050 I2C address (AD0 = 0, so address = 0x68)
#define MPU6050_I2C_ADDRESS 0x68

void TCA9548A_SelectChannelCallback(u8 u8Channel) {
    // This callback is called by the MPU6050 driver to select the appropriate channel
    if (u8Channel == 0xFF) {
        // Special case: disable all channels
        TCA9548A_DisableAllChannels();
    } else if (u8Channel < 5) {  // Only allow valid sensor channels 0-4
        // Select the specific channel with error checking
        TCA9548A_Status_t status = TCA9548A_SelectChannel(u8Channel);
        if (status != TCA9548A_OK) {
            // Log error but don't halt system
            UART_voidSendString("Channel select failed: ");
            UART_voidSendNumber(u8Channel);
            UART_voidSendString("\r\n");
        }
    }
    // Add small delay for multiplexer settling
    _delay_ms(2);
}

TCA9548A_Status_t TCA9548A_InitializeSmartGloveSystem(void) {
    TCA9548A_Status_t status;
    
    // Initialize the TCA9548A multiplexer
    status = TCA9548A_Init();
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // Register the callback function with MPU6050 driver
    MPU6050_voidSetMuxSelector(TCA9548A_SelectChannelCallback);
    
    // Configure sensor devices for each finger using MPU6050_Dev_t
    MPU6050_Dev_t sensor_configs[5] = {
        // Thumb sensor (Channel 0)
        {
            .u8I2cAddr = MPU6050_I2C_ADDRESS,
            .u8MuxChannel = TCA9548A_THUMB_CHANNEL,
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Index finger sensor (Channel 1)
        {
            .u8I2cAddr = MPU6050_I2C_ADDRESS,
            .u8MuxChannel = TCA9548A_INDEX_CHANNEL,
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Middle finger sensor (Channel 2)
        {
            .u8I2cAddr = MPU6050_I2C_ADDRESS,
            .u8MuxChannel = TCA9548A_MIDDLE_CHANNEL,
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Ring finger sensor (Channel 3)
        {
            .u8I2cAddr = MPU6050_I2C_ADDRESS,
            .u8MuxChannel = TCA9548A_RING_CHANNEL,
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Palm/hand orientation sensor (Channel 4)
        {
            .u8I2cAddr = MPU6050_I2C_ADDRESS,
            .u8MuxChannel = TCA9548A_PALM_CHANNEL,
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        }
    };
    
    // Register all sensors with the MPU6050 driver
    for (u8 i = 0; i < 5; i++) {
        MPU6050_voidRegisterSensor(i, &sensor_configs[i]);
    }
    
    // Initialize all sensors
    MPU6050_voidInitAllSensors();
    
    return TCA9548A_OK;
}

u8 TCA9548A_TestAllSensors(void) {
    u8 sensors_found = 0;
    TCA9548A_Status_t status;
    
    for (u8 channel = 0; channel < 5; channel++) {
        // Select the channel
        status = TCA9548A_SelectChannel(channel);
        if (status != TCA9548A_OK) {
            continue; // Just skip failed channels
        }
        
        _delay_ms(10); // Small delay after channel selection
        
        // Try to read WHO_AM_I register from MPU6050
        // Use direct I2C call since TCA9548A_ReadRegister doesn't actually select channels
        u8 who_am_i;
        if (I2C_u8ReadRegs(MPU6050_I2C_ADDRESS, 0x75, &who_am_i, 1) == 0) {
            if (who_am_i == 0x68) {
                sensors_found++;
            }
        }
    }
    
    // Disable all channels after testing
    TCA9548A_DisableAllChannels();
    
    return sensors_found;
}