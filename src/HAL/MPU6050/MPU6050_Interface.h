#ifndef MPU6050_INTERFACE_H
#define MPU6050_INTERFACE_H

#include "STD_TYPES.h"

// MPU6050 I2C Address
#define MPU6050_ADDRESS 0x68

// Function prototypes
void MPU6050_voidInit(void);
void MPU6050_voidReadAccel(s16* s16AccelX, s16* s16AccelY, s16* s16AccelZ);
void MPU6050_voidReadGyro(s16* s16GyroX, s16* s16GyroY, s16* s16GyroZ);
void MPU6050_voidReadTemp(s16* s16Temp);

#endif