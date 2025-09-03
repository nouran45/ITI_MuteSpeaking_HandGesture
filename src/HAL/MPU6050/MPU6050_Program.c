#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/I2C/I2C_Interface.h"
#include "MPU6050_Interface.h"
#include "MPU6050_Config.h"

// Private function to write to MPU6050 register
static void MPU6050_voidWriteRegister(u8 u8Reg, u8 u8Data) {
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 0); // Write operation
    I2C_voidWrite(u8Reg);
    I2C_voidWrite(u8Data);
    I2C_voidStop();
}

// Private function to read from MPU6050 register
static u8 MPU6050_u8ReadRegister(u8 u8Reg) {
    u8 u8Data;
    
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 0); // Write operation
    I2C_voidWrite(u8Reg);
    
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 1); // Read operation
    u8Data = I2C_u8ReadWithNACK();
    I2C_voidStop();
    
    return u8Data;
}

void MPU6050_voidInit(void) {
    // Wake up MPU6050
    MPU6050_voidWriteRegister(MPU6050_REG_PWR_MGMT_1, MPU6050_PWR_WAKEUP);
    
    // Set sample rate to 1kHz
    MPU6050_voidWriteRegister(MPU6050_REG_SMPLRT_DIV, 0x07);
    
    // Set accelerometer range to ±2g
    MPU6050_voidWriteRegister(MPU6050_REG_ACCEL_CONFIG, MPU6050_ACCEL_FS_2G);
    
    // Set gyroscope range to ±250°/s
    MPU6050_voidWriteRegister(MPU6050_REG_GYRO_CONFIG, MPU6050_GYRO_FS_250);
    
    // Configure low pass filter
    MPU6050_voidWriteRegister(MPU6050_REG_CONFIG, 0x06);
}

void MPU6050_voidReadAccel(s16* s16AccelX, s16* s16AccelY, s16* s16AccelZ) {
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 0); // Write operation
    I2C_voidWrite(MPU6050_REG_ACCEL_XOUT_H);
    
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 1); // Read operation
    
    *s16AccelX = (I2C_u8ReadWithACK() << 8) | I2C_u8ReadWithACK();
    *s16AccelY = (I2C_u8ReadWithACK() << 8) | I2C_u8ReadWithACK();
    *s16AccelZ = (I2C_u8ReadWithACK() << 8) | I2C_u8ReadWithNACK();
    
    I2C_voidStop();
}

void MPU6050_voidReadGyro(s16* s16GyroX, s16* s16GyroY, s16* s16GyroZ) {
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 0); // Write operation
    I2C_voidWrite(MPU6050_REG_GYRO_XOUT_H);
    
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 1); // Read operation
    
    *s16GyroX = (I2C_u8ReadWithACK() << 8) | I2C_u8ReadWithACK();
    *s16GyroY = (I2C_u8ReadWithACK() << 8) | I2C_u8ReadWithACK();
    *s16GyroZ = (I2C_u8ReadWithACK() << 8) | I2C_u8ReadWithNACK();
    
    I2C_voidStop();
}

void MPU6050_voidReadTemp(s16* s16Temp) {
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 0); // Write operation
    I2C_voidWrite(MPU6050_REG_TEMP_OUT_H);
    
    I2C_voidStart();
    I2C_voidWrite((MPU6050_ADDRESS << 1) | 1); // Read operation
    
    *s16Temp = (I2C_u8ReadWithACK() << 8) | I2C_u8ReadWithNACK();
    
    I2C_voidStop();
}