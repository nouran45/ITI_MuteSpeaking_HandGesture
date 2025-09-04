# Refactoring Update & New Files

* MCU: ATmega32A
* Clock: F_CPU = 8 MHz
* Toolchain: avr-gcc
* Types: project STD_TYPES.h (u8, u16, u32, s16, …)

# Goals

* Add multi-sensor support to the MPU6050 driver
* Hide raw I2C transactions behind tiny helpers
* Normalize headers and function prototypes
* Isolate tests so they do not collide with the app main
* Document structure and migration

# New Files

## I2C Helpers

* Path: src/MCAL/I2C/i2c_helpers.h
* Path: src/MCAL/I2C/i2c_helpers.c
* Purpose: wrap START, SLA, ACK, NACK, STOP into simple calls so device drivers stay clean
* Public API
  * I2C_helpers_init(u32 i2c_hz)
  * I2C_u8ReadRegs(u8 addr7, u8 reg, u8* buf, u8 len)
  * I2C_u8WriteReg(u8 addr7, u8 reg, u8 val)
* Characteristics
  * Uses AVR TWI registers directly
  * Blocking, master-only, 7-bit addressing
  * Returns 0 on success, 1 on error

## MPU6050 Split

* Path: src/HAL/MPU6050/mpu6050_types.h
  * mpu6050_status_t values: MPU6050_OK, MPU6050_E_BADID, MPU6050_E_I2C, MPU6050_E_WHOAMI
  * mpu6050_dev_t fields: i2c_addr, mux_channel, accel offsets, gyro offsets
  * mpu_mux_select_fn callback taking u8 channel
* Path: src/HAL/MPU6050/MPU6050_Config.h
  * Defines: MPU6050_MAX_SENSORS
  * Register constants: MPU6050_REG_* and defaults
  * Full-scale defaults: MPU6050_GYRO_FS_250, MPU6050_ACCEL_FS_2G
* Path: src/HAL/MPU6050/MPU6050_Interface.h
  * MPU6050_setMuxSelector(mpu_mux_select_fn fn)
  * MPU6050_registerSensor(u8 id, const mpu6050_dev_t* cfg)
  * MPU6050_initSensor(u8 id) returns mpu6050_status_t
  * MPU6050_initAllSensors(void)
  * MPU6050_readAll(u8 id, s16* ax, s16* ay, s16* az, s16* t, s16* gx, s16* gy, s16* gz) returns mpu6050_status_t
  * MPU6050_voidReadAccel(u8 id, s16* ax, s16* ay, s16* az)
  * MPU6050_voidReadGyro(u8 id, s16* gx, s16* gy, s16* gz)
  * MPU6050_voidCalculateAngles(u8 id, float* roll, float* pitch)
  * MPU6050_voidReadAllSensorsAngles(float* rollV, float* pitchV)
  * MPU6050_voidCalibrateSensor(u8 id)
  * MPU6050_voidCalibrateAllSensors(void)

# Modified Files

## src/HAL/MPU6050/MPU6050_Program.c

* Internal registry of sensors using a static array sized by MPU6050_MAX_SENSORS
* Optional mux callback invoked before each I2C transaction
* Uses I2C_u8ReadRegs and I2C_u8WriteReg for all register access
* Adds simple calibration that averages N samples into offsets
* Angle math includes a guard for M_PI

## Build System

* Root Makefile includes src/MCAL/I2C/i2c_helpers.c in sources
* Compiler flag added: -std=gnu99
* Keep tests out of the app build

## Tests

* Separate tests Makefile builds individual test ELF and HEX files
* Mark tests/*.c as Exclude from Build in the IDE
