#include "STD_TYPES.h"
#include "MPU6050_INTERFACE.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Integration.h"

// Define the function to register all sensors with the MPU6050 driver
void MPU6050_RegisterAllSensors(void) {
    // Define all five sensor configurations
    MPU6050_Dev_t sensors[5] = {
        // Sensor 0: Thumb
        {
            .u8I2cAddr = MPU6050_ADDRESS,  // 0x68 default
            .u8MuxChannel = 0,             // Thumb on channel 0
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 1: Index finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 1,             // Index on channel 1
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 2: Middle finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 2,             // Middle on channel 2
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 3: Ring finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 3,             // Ring on channel 3
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 4: Pinky finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 4,             // Pinky on channel 4
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        }
    };

    // Register all sensors with the MPU6050 driver
    for (u8 i = 0; i < 5; i++) {
        MPU6050_voidRegisterSensor(i, &sensors[i]);
    }
}
