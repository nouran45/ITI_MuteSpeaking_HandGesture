# Fix for "read fail" Issue in LCD_MPU Project

## Problem Identified
The main issue causing the "read fail" error is that the MPU6050 sensors are not being registered with the MPU6050 driver. The `smart_glove_init()` function in `gesture_stream_platform.c` calls `MPU6050_voidInitAllSensors()`, but this function only initializes sensors that have been previously registered using `MPU6050_voidRegisterSensor()`.

Additionally, the multiplexer selector function needs to be properly set up to enable communication with the different sensors on different multiplexer channels.

## Solution Implemented

1. Created a new function `MPU6050_RegisterAllSensors()` in the `missing_registration.c` file that registers all five sensors (one for each finger) with the MPU6050 driver.

2. Updated `TCA9548A_InitializeSmartGloveSystem()` in `TCA9548A_Integration.c` to call this new registration function.

3. Set up the multiplexer selector function using `MPU6050_voidSetMuxSelector(TCA9548A_SelectChannelCallback)` to handle channel switching.

## Implementation Details

### 1. The `missing_registration.c` file contains:
```c
#include "STD_TYPES.h"
#include "MPU6050_INTERFACE.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Integration.h"

// Define the function to register all sensors with the MPU6050 driver
void MPU6050_RegisterAllSensors(void) {
    // Define all five sensor configurations
    MPU6050_Dev_t sensors[5] = {
        // Sensor 0: Thumb
        {
            .u8I2cAddr = MPU6050_ADDRESS,  // 0x68 default
            .u8MuxChannel = 0,             // Thumb on channel 0
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 1: Index finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 1,             // Index on channel 1
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 2: Middle finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 2,             // Middle on channel 2
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 3: Ring finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 3,             // Ring on channel 3
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        },
        // Sensor 4: Pinky finger
        {
            .u8I2cAddr = MPU6050_ADDRESS,
            .u8MuxChannel = 4,             // Pinky on channel 4
            .s16AxOff = 0, .s16AyOff = 0, .s16AzOff = 0,
            .s16GxOff = 0, .s16GyOff = 0, .s16GzOff = 0
        }
    };

    // Register all sensors with the MPU6050 driver
    for (u8 i = 0; i < 5; i++) {
        MPU6050_voidRegisterSensor(i, &sensors[i]);
    }
}
```

### 2. Updates to `TCA9548A_Integration.c`:
```c
// Added the function call to register sensors
TCA9548A_Status_t TCA9548A_InitializeSmartGloveSystem(void){
    TCA9548A_Status_t st = TCA9548A_Init();
    if (st != TCA9548A_OK) return st;

    // Register callback with MPU driver for channel switching
    MPU6050_voidSetMuxSelector(TCA9548A_SelectChannelCallback);
    
    // Register all sensors with the MPU6050 driver
    MPU6050_RegisterAllSensors();

    // Basic per-channel bring-up: ensure WHO_AM_I responds on 0..4
    for (u8 ch=0; ch<5; ch++){
        u8 who=0;
        if (TCA9548A_ReadRegister(ch, MPU6050_I2C_ADDRESS, WHO_AM_I_REG, &who) != TCA9548A_OK || who!=0x68){
            UART_voidSendString("MPU missing on CH"); UART_voidSendNumber(ch); UART_voidSendString("\r\n");
        }
    }
    return TCA9548A_OK;
}
```

### 3. Updates to `TCA9548A_Integration.h`:
```c
void MPU6050_RegisterAllSensors(void);
```

### 4. Updated `Makefile` to include the new file:
```makefile
SOURCES = main_gesture_stream.c \
          DIO_Program.c \
          I2C_Program.c \
          LCD_Program.c \
          UART_Program.c \
          MPU6050_PROGRAM.c \
          TCA9548A_Program.c \
          TCA9548A_Integration.c \
          gesture_stream_platform.c \
          math_utils.c \
          missing_registration.c
```

## Building and Flashing Instructions
1. Make sure the build directory exists:
   ```
   mkdir -p build
   ```

2. Compile the code:
   ```
   make all
   ```

3. Flash the code to the microcontroller:
   ```
   make flash
   ```

## Expected Results
After implementing these changes, the system should properly register all sensors with the MPU6050 driver and set up the multiplexer selector function. This should resolve the "read fail" error message and allow the system to read data from the MPU6050 sensors correctly.
