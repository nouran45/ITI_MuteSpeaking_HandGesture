#include "STD_TYPES.h"
#include "TCA9548A_Integration.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Config.h"
#include "MPU6050_Interface.h"
#include "mpu6050_types.h"
#include "UART_Interface.h"
#include <util/delay.h>

// MPU6050 I2C address (AD0 = 0, so address = 0x68)
#define MPU6050_I2C_ADDRESS 0x68

void TCA9548A_SelectChannelCallback(u8 u8Channel) {
    // This callback is called by the MPU6050 driver to select the appropriate channel
    if (u8Channel == 0xFF) {
        // Special case: disable all channels
        TCA9548A_DisableAllChannels();
    } else {
        // Select the specific channel
        TCA9548A_SelectChannel(u8Channel);
    }
}

TCA9548A_Status_t TCA9548A_InitializeSmartGloveSystem(void) {
    TCA9548A_Status_t status;
    
    // Initialize the TCA9548A multiplexer
    status = TCA9548A_Init();
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // Register the callback function with MPU6050 driver
    MPU6050_setMuxSelector(TCA9548A_SelectChannelCallback);
    
    // Configure sensor devices for each finger
    mpu6050_dev_t sensor_configs[5] = {
        // Thumb sensor (Channel 0)
        {
            .i2c_addr = MPU6050_I2C_ADDRESS,
            .mux_channel = TCA9548A_THUMB_CHANNEL,
            .ax_off = 0, .ay_off = 0, .az_off = 0,
            .gx_off = 0, .gy_off = 0, .gz_off = 0
        },
        // Index finger sensor (Channel 1)
        {
            .i2c_addr = MPU6050_I2C_ADDRESS,
            .mux_channel = TCA9548A_INDEX_CHANNEL,
            .ax_off = 0, .ay_off = 0, .az_off = 0,
            .gx_off = 0, .gy_off = 0, .gz_off = 0
        },
        // Middle finger sensor (Channel 2)
        {
            .i2c_addr = MPU6050_I2C_ADDRESS,
            .mux_channel = TCA9548A_MIDDLE_CHANNEL,
            .ax_off = 0, .ay_off = 0, .az_off = 0,
            .gx_off = 0, .gy_off = 0, .gz_off = 0
        },
        // Ring finger sensor (Channel 3)
        {
            .i2c_addr = MPU6050_I2C_ADDRESS,
            .mux_channel = TCA9548A_RING_CHANNEL,
            .ax_off = 0, .ay_off = 0, .az_off = 0,
            .gx_off = 0, .gy_off = 0, .gz_off = 0
        },
        // Palm/hand orientation sensor (Channel 4)
        {
            .i2c_addr = MPU6050_I2C_ADDRESS,
            .mux_channel = TCA9548A_PALM_CHANNEL,
            .ax_off = 0, .ay_off = 0, .az_off = 0,
            .gx_off = 0, .gy_off = 0, .gz_off = 0
        }
    };
    
    // Register all sensors with the MPU6050 driver
    for (u8 i = 0; i < 5; i++) {
        MPU6050_registerSensor(i, &sensor_configs[i]);
    }
    
    // Initialize all sensors
    MPU6050_initAllSensors();
    
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
        u8 who_am_i;
        status = TCA9548A_ReadRegister(channel, MPU6050_I2C_ADDRESS, 0x75, &who_am_i);
        
        if (status == TCA9548A_OK && who_am_i == 0x68) {
            sensors_found++;
        }
    }
    
    // Disable all channels after testing
    TCA9548A_DisableAllChannels();
    
    return sensors_found;
}