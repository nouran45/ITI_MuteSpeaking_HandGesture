#ifndef MPU6050_INTERFACE_H
#define MPU6050_INTERFACE_H

#include "STD_TYPES.h"


/***************************** MPU6050 I2C Address *****************************/
#define MPU6050_ADDRESS 0x68

/***************************** Type Definitions *****************************/

// External I2C mux selector function pointer (e.g., TCA9548A). Pass 0xFF in channel to disable.
typedef void (*mpu_mux_select_fn)(u8 u8ChannelCopy);

// MPU6050 device configuration structure
typedef struct {
    u8  u8I2cAddr;        // 0x68 or 0x69
    u8  u8MuxChannel;     // 0..7 or 0xFF if no mux
    s16 s16AxOff, s16AyOff, s16AzOff;  // accel offset (raw LSB)
    s16 s16GxOff, s16GyOff, s16GzOff;  // gyro offset (raw LSB)
} MPU6050_Dev_t;

// MPU6050 slot structure for device management
typedef struct {
    MPU6050_Dev_t stDev;
    u8 u8Registered;
} MPU6050_Slot_t;

// Raw sensor data structure
typedef struct {
    s16 s16Ax, s16Ay, s16Az;  // accelerometer raw values
    s16 s16Gx, s16Gy, s16Gz;  // gyroscope raw values
    s16 s16Temp;              // temperature raw value
} MPU6050_RawData_t;

// Status enumeration
typedef enum {
    MPU6050_OK = 0,
    MPU6050_E_I2C = 1,
    MPU6050_E_WHOAMI = 2,
    MPU6050_E_BADID = 3
} MPU6050_Status_t;

/***************************** Function Prototypes *****************************/

// Mux configuration
void MPU6050_voidSetMuxSelector(mpu_mux_select_fn fn);

// Sensor registration and initialization
void MPU6050_voidRegisterSensor(u8 u8IdCopy, const MPU6050_Dev_t* pstDevCpy);
MPU6050_Status_t MPU6050_enumInitSensor(u8 u8IdCopy);
void MPU6050_voidInitAllSensors(void);

// Data reading functions
MPU6050_Status_t MPU6050_enumReadSensorData(u8 u8IdCopy, MPU6050_RawData_t* pstDataCpy);
MPU6050_Status_t MPU6050_enumReadAll(u8 u8IdCopy, s16* ps16AxCpy, s16* ps16AyCpy, s16* ps16AzCpy,
                                     s16* ps16TempCpy, s16* ps16GxCpy, s16* ps16GyCpy, s16* ps16GzCpy);
void MPU6050_voidReadAccel(u8 u8IdCopy, s16* ps16AxCpy, s16* ps16AyCpy, s16* ps16AzCpy);
void MPU6050_voidReadGyro(u8 u8IdCopy, s16* ps16GxCpy, s16* ps16GyCpy, s16* ps16GzCpy);

// Calibration functions
void MPU6050_voidCalibrateSensor(u8 u8IdCopy);
void MPU6050_voidCalibrateAllSensors(void);

#endif /* MPU_INTERFACE_H */