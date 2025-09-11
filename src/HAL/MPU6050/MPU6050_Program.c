#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "MPU6050_Interface.h"
#include "MPU6050_Config.h"
#include "mpu6050_types.h"
#include "../../MCAL/I2C/i2c_helpers.h"
#include "UART_Interface.h" // optional for logs
#include <math.h>



typedef struct { mpu6050_dev_t dev; u8 registered; } mpu6050_slot_t;
static mpu6050_slot_t g_sensors[MPU6050_MAX_SENSORS];
static mpu_mux_select_fn s_mux = 0;


void MPU6050_voidInit(void){ (void)MPU6050_initSensor(0); }

static inline void select_channel(const mpu6050_dev_t* d){
    if(s_mux && d->mux_channel != 0xFF) s_mux(d->mux_channel);
}

static inline s16 be16(const u8* p){ return (s16)((p[0]<<8)|p[1]); }

void MPU6050_setMuxSelector(mpu_mux_select_fn fn){ s_mux = fn; }

void MPU6050_registerSensor(u8 id, const mpu6050_dev_t* cfg){
    if(id<MPU6050_MAX_SENSORS && cfg){ g_sensors[id].dev = *cfg; g_sensors[id].registered = 1; }
}

mpu6050_status_t MPU6050_initSensor(u8 id){
    if(id>=MPU6050_MAX_SENSORS || !g_sensors[id].registered) return MPU6050_E_BADID;
    mpu6050_dev_t* d = &g_sensors[id].dev;

    select_channel(d);

    // WHO_AM_I
    u8 who=0xFF;
    if(I2C_u8ReadRegs(d->i2c_addr, 0x75, &who, 1)) return MPU6050_E_I2C;
    if(who != 0x68) return MPU6050_E_WHOAMI;

    // Wake + config
    if(I2C_u8WriteReg(d->i2c_addr, MPU6050_REG_PWR_MGMT_1, MPU6050_PWR_WAKEUP)) return MPU6050_E_I2C;
    if(I2C_u8WriteReg(d->i2c_addr, MPU6050_REG_SMPLRT_DIV, 0x07))               return MPU6050_E_I2C;
    if(I2C_u8WriteReg(d->i2c_addr, MPU6050_REG_CONFIG,       0x03))             return MPU6050_E_I2C;
    if(I2C_u8WriteReg(d->i2c_addr, MPU6050_REG_GYRO_CONFIG,  MPU6050_GYRO_FS_250)) return MPU6050_E_I2C;
    if(I2C_u8WriteReg(d->i2c_addr, MPU6050_REG_ACCEL_CONFIG, MPU6050_ACCEL_FS_2G)) return MPU6050_E_I2C;

    return MPU6050_OK;
}

void MPU6050_initAllSensors(void){
    for(u8 i=0;i<MPU6050_MAX_SENSORS;i++){
        if(g_sensors[i].registered) (void)MPU6050_initSensor(i);
    }
}

static u8 burst14(u8 id, u8* b){
    if(id>=MPU6050_MAX_SENSORS || !g_sensors[id].registered) return 1;
    const mpu6050_dev_t* d = &g_sensors[id].dev;
    select_channel(d);
    return I2C_u8ReadRegs(d->i2c_addr, MPU6050_REG_ACCEL_XOUT_H, b, 14);
}

mpu6050_status_t MPU6050_readAll(u8 id, s16* ax, s16* ay, s16* az,
                                       s16* t,  s16* gx, s16* gy, s16* gz){
    u8 b[14]; if(burst14(id,b)) return MPU6050_E_I2C;
    const mpu6050_dev_t* d = &g_sensors[id].dev;
    s16 AX=be16(b+0), AY=be16(b+2), AZ=be16(b+4);
    s16 TT=be16(b+6), GX=be16(b+8), GY=be16(b+10), GZ=be16(b+12);
    if(ax) *ax = AX - d->ax_off; 
    if(ay) *ay = AY - d->ay_off; 
    if(az) *az = AZ - d->az_off;
    if(t ) *t  = TT;
    if(gx) *gx = GX - d->gx_off; 
    if(gy) *gy = GY - d->gy_off; 
    if(gz) *gz = GZ - d->gz_off;
    return MPU6050_OK;
}

void MPU6050_voidReadAccel(u8 id, s16* ax, s16* ay, s16* az){
    (void)MPU6050_readAll(id, ax, ay, az, 0, 0, 0, 0);
}
void MPU6050_voidReadGyro(u8 id, s16* gx, s16* gy, s16* gz){
    (void)MPU6050_readAll(id, 0, 0, 0, 0, gx, gy, gz);
}

void MPU6050_voidCalculateAngles(u8 id, float* roll, float* pitch){
    s16 ax,ay,az; MPU6050_voidReadAccel(id,&ax,&ay,&az);
    const float g = 16384.0f;
    float axg = ax/g, ayg = ay/g, azg = az/g;
    if(roll)  *roll  = (180.0f/M_PI) * atanf( ayg / azg );
    if(pitch) *pitch = (180.0f/M_PI) * atanf(-axg / sqrtf(ayg*ayg + azg*azg));
}

void MPU6050_voidReadAllSensorsAngles(float* rollV, float* pitchV){
    for(u8 i=0;i<MPU6050_MAX_SENSORS;i++){
        if(!g_sensors[i].registered) continue;
        MPU6050_voidCalculateAngles(i, rollV? &rollV[i]:0, pitchV? &pitchV[i]:0);
    }
}

void MPU6050_voidCalibrateSensor(u8 id){
    if(id>=MPU6050_MAX_SENSORS || !g_sensors[id].registered) return;
    long ax=0,ay=0,az=0,gx=0,gy=0,gz=0; u8 b[14];
    const u16 N=256;
    for(u16 i=0;i<N;i++){
        if(burst14(id,b)) continue;
        ax += be16(b+0); ay += be16(b+2); az += be16(b+4);
        gx += be16(b+8); gy += be16(b+10); gz += be16(b+12);
    }
    mpu6050_dev_t* d=&g_sensors[id].dev;
    d->ax_off=(s16)(ax/N); d->ay_off=(s16)(ay/N); d->az_off=(s16)((az/N)-16384); // remove 1g if upright
    d->gx_off=(s16)(gx/N); d->gy_off=(s16)(gy/N); d->gz_off=(s16)(gz/N);
}

void MPU6050_voidCalibrateAllSensors(void){
    for(u8 i=0;i<MPU6050_MAX_SENSORS;i++){
        if(g_sensors[i].registered) MPU6050_voidCalibrateSensor(i);
    }
}

mpu6050_status_t MPU6050_readSensorData(u8 id, mpu6050_raw_data_t* data) {
    if (!data) return MPU6050_E_BADID;
    return MPU6050_readAll(id, &data->ax, &data->ay, &data->az, &data->temp, &data->gx, &data->gy, &data->gz);
}