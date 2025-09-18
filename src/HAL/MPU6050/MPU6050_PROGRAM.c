#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "MPU6050_INTERFACE.h"
#include "MPU6050_config.h"
#include "I2C_Interface.h"
#include <util/delay.h>

MPU6050_Slot_t MPU6050_astSensors[MPU6050_MAX_SENSORS];
static mpu_mux_select_fn MPU6050_pfnMuxSelector = 0;

static void MPU6050_voidSelectChannel(const MPU6050_Dev_t* pstDevCpy) {
    if(MPU6050_pfnMuxSelector && pstDevCpy->u8MuxChannel != 0xFF) {
        MPU6050_pfnMuxSelector(pstDevCpy->u8MuxChannel);
        _delay_us(10);
    }
}

static s16 MPU6050_s16Be16(const u8* pu8DataCpy) {
    return (s16)((pu8DataCpy[0] << 8) | pu8DataCpy[1]);
}

static u8 MPU6050_u8Burst14(u8 u8IdCopy, u8* pu8BufferCpy) {
    if(u8IdCopy >= MPU6050_MAX_SENSORS || !MPU6050_astSensors[u8IdCopy].u8Registered) {
        return 1;
    }

    const MPU6050_Dev_t* pstDev = &MPU6050_astSensors[u8IdCopy].stDev;
    MPU6050_voidSelectChannel(pstDev);

    // Use new I2C functions
    if (I2C_u8Start()) return 1;
    if (TWI_WriteByteRaw(pstDev->u8I2cAddr << 1)) {
        I2C_voidStop();
        return 1;
    }
    if (TWI_WriteByteRaw(MPU6050_REG_ACCEL_XOUT_H)) {
        I2C_voidStop();
        return 1;
    }
    I2C_voidStop();
    _delay_us(10);

    if (I2C_u8Start()) return 1;
    if (TWI_WriteByteRaw((pstDev->u8I2cAddr << 1) | 1)) {
        I2C_voidStop();
        return 1;
    }

    for(u8 i = 0; i < 13; i++) {
        pu8BufferCpy[i] = I2C_u8ReadWithACK();
    }
    pu8BufferCpy[13] = I2C_u8ReadWithNACK();

    I2C_voidStop();
    return 0;
}

void MPU6050_voidSetMuxSelector(mpu_mux_select_fn fn) {
    MPU6050_pfnMuxSelector = fn;
}

void MPU6050_voidRegisterSensor(u8 u8IdCopy, const MPU6050_Dev_t* pstDevCpy) {
    if(u8IdCopy < MPU6050_MAX_SENSORS && pstDevCpy) {
        MPU6050_astSensors[u8IdCopy].stDev = *pstDevCpy;
        MPU6050_astSensors[u8IdCopy].u8Registered = 1;
    }
}

MPU6050_Status_t MPU6050_enumInitSensor(u8 u8IdCopy) {
    if(u8IdCopy >= MPU6050_MAX_SENSORS || !MPU6050_astSensors[u8IdCopy].u8Registered) {
        return MPU6050_E_BADID;
    }

    MPU6050_Dev_t* pstDev = &MPU6050_astSensors[u8IdCopy].stDev;
    MPU6050_voidSelectChannel(pstDev);

    // Check WHO_AM_I
    u8 who_am_i;
    if (I2C_u8ReadRegs(pstDev->u8I2cAddr, MPU6050_REG_WHO_AM_I, &who_am_i, 1)) {
        return MPU6050_E_I2C;
    }

    if(who_am_i != 0x68) {
        return MPU6050_E_WHOAMI;
    }

    // Initialize sensor
    if (I2C_u8WriteReg(pstDev->u8I2cAddr, MPU6050_REG_PWR_MGMT_1, MPU6050_PWR_WAKEUP)) {
        return MPU6050_E_I2C;
    }
    _delay_ms(10);

    if (I2C_u8WriteReg(pstDev->u8I2cAddr, MPU6050_REG_GYRO_CONFIG, MPU6050_GYRO_FS_250)) {
        return MPU6050_E_I2C;
    }
    _delay_ms(10);

    if (I2C_u8WriteReg(pstDev->u8I2cAddr, MPU6050_REG_ACCEL_CONFIG, MPU6050_ACCEL_FS_2G)) {
        return MPU6050_E_I2C;
    }
    _delay_ms(10);

    return MPU6050_OK;
}

void MPU6050_voidInitAllSensors(void) {
    for(u8 i = 0; i < MPU6050_MAX_SENSORS; i++) {
        if(MPU6050_astSensors[i].u8Registered) {
            MPU6050_enumInitSensor(i);
        }
    }
}

MPU6050_Status_t MPU6050_enumReadAll(u8 u8IdCopy, s16* ps16AxCpy, s16* ps16AyCpy, s16* ps16AzCpy,
                                    s16* ps16TempCpy, s16* ps16GxCpy, s16* ps16GyCpy, s16* ps16GzCpy) {
    u8 u8Buffer[14];
    if(MPU6050_u8Burst14(u8IdCopy, u8Buffer)) {
        return MPU6050_E_I2C;
    }

    const MPU6050_Dev_t* pstDev = &MPU6050_astSensors[u8IdCopy].stDev;

    if(ps16AxCpy) *ps16AxCpy = MPU6050_s16Be16(u8Buffer+0) - pstDev->s16AxOff;
    if(ps16AyCpy) *ps16AyCpy = MPU6050_s16Be16(u8Buffer+2) - pstDev->s16AyOff;
    if(ps16AzCpy) *ps16AzCpy = MPU6050_s16Be16(u8Buffer+4) - pstDev->s16AzOff;
    if(ps16TempCpy) *ps16TempCpy = MPU6050_s16Be16(u8Buffer+6);
    if(ps16GxCpy) *ps16GxCpy = MPU6050_s16Be16(u8Buffer+8) - pstDev->s16GxOff;
    if(ps16GyCpy) *ps16GyCpy = MPU6050_s16Be16(u8Buffer+10) - pstDev->s16GyOff;
    if(ps16GzCpy) *ps16GzCpy = MPU6050_s16Be16(u8Buffer+12) - pstDev->s16GzOff;

    return MPU6050_OK;
}

void MPU6050_voidReadAccel(u8 u8IdCopy, s16* ps16AxCpy, s16* ps16AyCpy, s16* ps16AzCpy) {
    MPU6050_enumReadAll(u8IdCopy, ps16AxCpy, ps16AyCpy, ps16AzCpy, NULL, NULL, NULL, NULL);
}

void MPU6050_voidReadGyro(u8 u8IdCopy, s16* ps16GxCpy, s16* ps16GyCpy, s16* ps16GzCpy) {
    MPU6050_enumReadAll(u8IdCopy, NULL, NULL, NULL, NULL, ps16GxCpy, ps16GyCpy, ps16GzCpy);
}

MPU6050_Status_t MPU6050_enumReadSensorData(u8 u8IdCopy, MPU6050_RawData_t* pstDataCpy) {
    if (!pstDataCpy) return MPU6050_E_BADID;
    return MPU6050_enumReadAll(u8IdCopy, &pstDataCpy->s16Ax, &pstDataCpy->s16Ay, &pstDataCpy->s16Az,
                              &pstDataCpy->s16Temp, &pstDataCpy->s16Gx, &pstDataCpy->s16Gy, &pstDataCpy->s16Gz);
}

void MPU6050_voidCalibrateSensor(u8 u8IdCopy) {
    if(u8IdCopy >= MPU6050_MAX_SENSORS || !MPU6050_astSensors[u8IdCopy].u8Registered) return;

    long s32Ax = 0, s32Ay = 0, s32Az = 0, s32Gx = 0, s32Gy = 0, s32Gz = 0;
    u8 u8Buffer[14];
    const u16 u16Samples = 100;

    for(u16 i = 0; i < u16Samples; i++) {
        if(MPU6050_u8Burst14(u8IdCopy, u8Buffer)) continue;
        s32Ax += MPU6050_s16Be16(u8Buffer+0);
        s32Ay += MPU6050_s16Be16(u8Buffer+2);
        s32Az += MPU6050_s16Be16(u8Buffer+4);
        s32Gx += MPU6050_s16Be16(u8Buffer+8);
        s32Gy += MPU6050_s16Be16(u8Buffer+10);
        s32Gz += MPU6050_s16Be16(u8Buffer+12);
        _delay_ms(10);
    }

    MPU6050_Dev_t* pstDev = &MPU6050_astSensors[u8IdCopy].stDev;
    pstDev->s16AxOff = (s16)(s32Ax / u16Samples);
    pstDev->s16AyOff = (s16)(s32Ay / u16Samples);
    pstDev->s16AzOff = (s16)((s32Az / u16Samples) - 16384);
    pstDev->s16GxOff = (s16)(s32Gx / u16Samples);
    pstDev->s16GyOff = (s16)(s32Gy / u16Samples);
    pstDev->s16GzOff = (s16)(s32Gz / u16Samples);
}

void MPU6050_voidCalibrateAllSensors(void) {
    for(u8 i = 0; i < MPU6050_MAX_SENSORS; i++) {
        if(MPU6050_astSensors[i].u8Registered) {
            MPU6050_voidCalibrateSensor(i);
        }
    }
}
