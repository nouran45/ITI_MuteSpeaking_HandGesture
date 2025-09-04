#ifndef MPU6050_INTERFACE_H
#define MPU6050_INTERFACE_H

#include "../../LIB/STD_TYPES.h"
#include "mpu6050_types.h"
#include "MPU6050_Config.h"

// MPU6050 I2C Address
#define MPU6050_ADDRESS 0x68

// Function prototypes
void MPU6050_voidInit(void);
void MPU6050_voidReadAccel(u8 id, s16* ax, s16* ay, s16* az);
void MPU6050_voidReadGyro (u8 id, s16* gx, s16* gy, s16* gz);

// EXACT prototype:
mpu6050_status_t MPU6050_initSensor(u8 id);

#endif
