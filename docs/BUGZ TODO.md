`If u are done with any bug remove it from file plz`


# MPU6050 Interface Issues Analysis
`These functions are implemented in MPU6050 Program and not declared in the interface file.`

## Missing Function Declarations (Need to be added to MPU6050_Interface.h):

### 1. Multi-Sensor Functions
- `MPU6050_voidReadAccel(u8 sensorId, s16* ax, s16* ay, s16* az)`
- `MPU6050_voidReadGyro(u8 sensorId, s16* gx, s16* gy, s16* gz)`
- `MPU6050_initSensor(u8 id)`
- `MPU6050_initAllSensors(void)`
- `MPU6050_registerSensor(u8 id, u8 i2c_addr)`

### 2. Calibration Functions  
- `MPU6050_voidCalibrateAccel(u8 sensorId)`
- `MPU6050_voidCalibrateGyro(u8 sensorId)`
- `MPU6050_voidCalibrateAllSensors(void)`

### 3. Angle Calculation Functions
- `MPU6050_voidCalculateAngles(s16 ax, s16 ay, s16 az, float* roll, float* pitch)`
- `MPU6050_voidReadAllSensorsAngles(float angles[][2])`

### 4. Multiplexer Integration
- `MPU6050_setMuxSelector(void (*callback)(u8))`

### 5. Status and Utility Functions
- `MPU6050_u8GetStatus(u8 sensorId)`
- `MPU6050_voidPrintSensorData(u8 sensorId)`