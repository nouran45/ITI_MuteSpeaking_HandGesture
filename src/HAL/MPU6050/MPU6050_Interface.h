#ifndef MPU6050_INTERFACE_H
#define MPU6050_INTERFACE_H

#include "../../LIB/STD_TYPES.h"
#include "mpu6050_types.h"
#include "MPU6050_Config.h"

// MPU6050 I2C Address
#define MPU6050_ADDRESS 0x68

/* ---------- Optional mux selector ---------- */
void MPU6050_setMuxSelector(mpu_mux_select_fn fn);

/* ---------- Registration / init ---------- */
void              MPU6050_registerSensor(u8 id, const mpu6050_dev_t* cfg);
mpu6050_status_t  MPU6050_initSensor(u8 id);
void              MPU6050_initAllSensors(void);


/* ---------- Data I/O ---------- */
mpu6050_status_t  MPU6050_readAll(u8 id,
                     s16* ax, s16* ay, s16* az,
                     s16* t,
                     s16* gx, s16* gy, s16* gz);

void MPU6050_voidReadAccel(u8 id, s16* ax, s16* ay, s16* az);
void MPU6050_voidReadGyro (u8 id, s16* gx, s16* gy, s16* gz);

/* ---------- Angles ---------- */
void MPU6050_voidCalculateAngles(u8 id, float* roll, float* pitch);
/* Vector form for all registered sensors; arrays must have size >= MPU6050_MAX_SENSORS */
void MPU6050_voidReadAllSensorsAngles(float* rollV, float* pitchV);

/* ---------- Calibration ---------- */
void MPU6050_voidCalibrateSensor(u8 id);
void MPU6050_voidCalibrateAllSensors(void);

/* ---------- (Optional pure-math helper, no I2C) ---------- */
void MPU6050_anglesFromAccel(s16 ax, s16 ay, s16 az, float* roll, float* pitch); 



#endif
