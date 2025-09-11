#ifndef MPU6050_TYPES_H
#define MPU6050_TYPES_H

#include "STD_TYPES.h"

// External I2C mux selector (e.g., TCA9548A). Pass 0xFF in dev to disable.
typedef void (*mpu_mux_select_fn)(u8 channel); //  supplies later

typedef struct {
    u8  i2c_addr;     // 0x68 or 0x69
    u8  mux_channel;  // 0..7 or 0xFF if no mux
    s16 ax_off, ay_off, az_off; // accel offset (raw LSB)
    s16 gx_off, gy_off, gz_off; // gyro offset (raw LSB)
} mpu6050_dev_t;

// Raw sensor data structure
typedef struct {
    s16 ax, ay, az;  // accelerometer raw values
    s16 gx, gy, gz;  // gyroscope raw values
    s16 temp;        // temperature raw value
} mpu6050_raw_data_t;

typedef enum {
    MPU6050_OK = 0,
    MPU6050_E_I2C = 1,
    MPU6050_E_WHOAMI = 2,
    MPU6050_E_BADID  = 3
} mpu6050_status_t;

typedef struct {
    s16 ax, ay, az; // Accelerometer
    s16 gx, gy, gz; // Gyroscope
    s16 temp;       // (optional) Temperature
} mpu6050_raw_data_t;


#endif
