# TCA9548A I2C Multiplexer Driver Documentation

## Overview

The TCA9548A is an 8-channel I2C multiplexer that allows multiple devices with the same I2C address to be connected to a single I2C bus. This is essential for the Smart Glove project where 5 MPU6050 sensors (all with address 0x68) need to be connected simultaneously.

## Features

- ✅ **Complete TCA9548A Driver Implementation**
- ✅ **8-Channel I2C Multiplexing Support** 
- ✅ **Individual and Multiple Channel Selection**
- ✅ **Hardware and Software Reset Functionality**
- ✅ **Error Handling and Status Reporting**
- ✅ **MPU6050 Integration with Callback System**
- ✅ **Smart Glove System Integration**
- ✅ **Communication Testing and Diagnostics**

## File Structure

```
src/HAL/TCA9548A/
├── TCA9548A_Config.h      - Configuration constants and channel mapping
├── TCA9548A_Interface.h   - Function declarations and API
├── TCA9548A_Program.c     - Core driver implementation
├── TCA9548A_Integration.h - Smart Glove system integration
└── TCA9548A_Integration.c - MPU6050 callback and system setup

tests/
└── TCA9548A_test.c        - Comprehensive test program
```

## Hardware Configuration

### I2C Address
- Default address: **0x70** (A2=A1=A0=0)
- Configurable via hardware address pins A0, A1, A2

### Channel Mapping for Smart Glove
```c
#define TCA9548A_THUMB_CHANNEL      0    // Channel 0 - Thumb sensor
#define TCA9548A_INDEX_CHANNEL      1    // Channel 1 - Index finger sensor  
#define TCA9548A_MIDDLE_CHANNEL     2    // Channel 2 - Middle finger sensor
#define TCA9548A_RING_CHANNEL       3    // Channel 3 - Ring finger sensor
#define TCA9548A_PALM_CHANNEL       4    // Channel 4 - Palm/hand orientation sensor
```

### Pin Connections
| TCA9548A Pin | ATmega32a Pin | Function |
|--------------|---------------|----------|
| SCL          | PC1           | I2C Clock |
| SDA          | PC0           | I2C Data |
| A0           | GND           | Address bit 0 |
| A1           | GND           | Address bit 1 |
| A2           | GND           | Address bit 2 |
| RESET        | PB0 (optional)| Hardware reset |

## API Reference

### Initialization
```c
TCA9548A_Status_t TCA9548A_Init(void);
```
Initializes the TCA9548A multiplexer and tests communication.

### Channel Selection
```c
TCA9548A_Status_t TCA9548A_SelectChannel(u8 u8ChannelId);
TCA9548A_Status_t TCA9548A_SelectMultipleChannels(u8 u8ChannelMask);
TCA9548A_Status_t TCA9548A_DisableAllChannels(void);
```

### Status and Testing
```c
u8 TCA9548A_GetActiveChannels(void);
TCA9548A_ChannelState_t TCA9548A_IsChannelActive(u8 u8ChannelId);
TCA9548A_Status_t TCA9548A_TestCommunication(void);
```

### Device Access
```c
TCA9548A_Status_t TCA9548A_ReadRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8* pu8Data);
TCA9548A_Status_t TCA9548A_WriteRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8 u8Data);
TCA9548A_Status_t TCA9548A_ReadMultipleRegisters(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8* pu8Data, u8 u8Length);
```

### Reset Functions
```c
TCA9548A_Status_t TCA9548A_HardwareReset(void);
TCA9548A_Status_t TCA9548A_SoftwareReset(void);
```

### Smart Glove Integration
```c
TCA9548A_Status_t TCA9548A_InitializeSmartGloveSystem(void);
u8 TCA9548A_TestAllSensors(void);
void TCA9548A_SelectChannelCallback(u8 u8Channel);
```

## Usage Examples

### Basic Initialization
```c
#include "TCA9548A_Interface.h"

int main(void) {
    // Initialize I2C bus first
    I2C_helpers_init(100000); // 100 kHz
    
    // Initialize TCA9548A
    TCA9548A_Status_t status = TCA9548A_Init();
    if (status != TCA9548A_OK) {
        // Handle error
        return -1;
    }
    
    // Select channel 0 (Thumb sensor)
    TCA9548A_SelectChannel(0);
    
    // Now you can communicate with devices on channel 0
    // ...
}
```

### Smart Glove System Setup
```c
#include "TCA9548A_Integration.h"

int main(void) {
    // Initialize complete Smart Glove system
    TCA9548A_Status_t status = TCA9548A_InitializeSmartGloveSystem();
    if (status != TCA9548A_OK) {
        // Handle initialization error
        return -1;
    }
    
    // Test all sensors
    u8 sensors_found = TCA9548A_TestAllSensors();
    printf("Found %d sensors out of 5\n", sensors_found);
    
    // Now you can use MPU6050 functions with sensor IDs
    s16 ax, ay, az;
    MPU6050_voidReadAccel(0, &ax, &ay, &az); // Read thumb sensor
    MPU6050_voidReadAccel(1, &ax, &ay, &az); // Read index finger sensor
    // ...
}
```

### Manual Channel Switching
```c
// Cycle through all 5 channels
for (u8 channel = 0; channel < 5; channel++) {
    TCA9548A_SelectChannel(channel);
    
    // Read WHO_AM_I from MPU6050 on this channel
    u8 who_am_i;
    TCA9548A_ReadRegister(channel, 0x68, 0x75, &who_am_i);
    
    if (who_am_i == 0x68) {
        printf("Sensor found on channel %d\n", channel);
    }
}
```

### Error Handling
```c
TCA9548A_Status_t status = TCA9548A_SelectChannel(2);
switch (status) {
    case TCA9548A_OK:
        printf("Channel 2 selected successfully\n");
        break;
    case TCA9548A_ERROR_I2C:
        printf("I2C communication error\n");
        break;
    case TCA9548A_ERROR_INVALID_CHANNEL:
        printf("Invalid channel number\n");
        break;
    case TCA9548A_ERROR_NOT_INITIALIZED:
        printf("TCA9548A not initialized\n");
        break;
    default:
        printf("Unknown error: %s\n", TCA9548A_GetStatusString(status));
}
```

## Testing and Diagnostics

### TCA9548A Test Program
Run the comprehensive test program to verify functionality:

```bash
cd tests
make TCA9548A_test
make flash_TCA9548A_test
```

The test program will:
1. Initialize all peripherals
2. Test TCA9548A communication
3. Scan all channels for MPU6050 sensors
4. Initialize the complete Smart Glove system
5. Demonstrate channel switching

### Expected Output
```
=================================
TCA9548A I2C Multiplexer Test
=================================
Initializing I2C bus...
Initializing TCA9548A multiplexer...
TCA9548A initialization successful!
Testing TCA9548A communication...
TCA9548A communication test passed!
Testing all sensors on TCA9548A channels...
Channel 0: MPU6050 found (WHO_AM_I=0x68)
Channel 1: MPU6050 found (WHO_AM_I=0x68)
Channel 2: MPU6050 found (WHO_AM_I=0x68)
Channel 3: MPU6050 found (WHO_AM_I=0x68)
Channel 4: MPU6050 found (WHO_AM_I=0x68)
Sensor scan complete. Found 5 sensors.
Smart Glove System initialized successfully!
```

## Integration with Existing Code

### MPU6050 Integration
The TCA9548A driver integrates seamlessly with your existing MPU6050 code:

```c
// Old single-sensor code:
MPU6050_voidReadAccel(&ax, &ay, &az);

// New multi-sensor code:
MPU6050_voidReadAccel(0, &ax, &ay, &az); // Sensor ID 0 (Thumb)
MPU6050_voidReadAccel(1, &bx, &by, &bz); // Sensor ID 1 (Index)
// etc.
```

### Callback System
The driver uses a callback system that automatically handles channel switching:

```c
// Set up the callback (done in TCA9548A_InitializeSmartGloveSystem)
MPU6050_setMuxSelector(TCA9548A_SelectChannelCallback);

// Now MPU6050 functions automatically switch channels as needed
MPU6050_voidReadAccel(2, &ax, &ay, &az); // Automatically selects channel 2
```

## Performance Considerations

- **Channel Switching Overhead**: ~1ms per channel switch
- **I2C Speed**: Configured for 100 kHz (can be increased to 400 kHz if needed)
- **Memory Usage**: ~200 bytes RAM, ~1.5KB flash
- **Optimization**: Channel selection is cached to avoid unnecessary I2C transactions

## Troubleshooting

### Common Issues

1. **TCA9548A Not Responding**
   - Check I2C wiring (SDA/SCL pull-ups required: 4.7kΩ)
   - Verify power supply (3.3V or 5V)
   - Check I2C address (default 0x70)

2. **Sensors Not Found**
   - Verify MPU6050 connections on each channel
   - Check individual sensor power supplies
   - Use oscilloscope to verify I2C signals

3. **Intermittent Communication**
   - Check for loose connections
   - Verify adequate power supply current
   - Consider reducing I2C speed

### Debug Functions
```c
// Test basic TCA9548A communication
TCA9548A_Status_t status = TCA9548A_TestCommunication();

// Scan all channels for devices  
u8 sensors_found = TCA9548A_TestAllSensors();

// Check current channel selection
u8 active_channels = TCA9548A_GetActiveChannels();
```

## Next Steps

With the TCA9548A driver completed, you can now:

1. ✅ **Connect 5 MPU6050 sensors simultaneously**
2. ✅ **Use existing MPU6050 functions with sensor IDs**
3. ➡️ **Implement Feature Extraction module**
4. ➡️ **Add ESP32 communication for gesture recognition**
5. ➡️ **Create state machine for different operation modes**

The foundation for multi-sensor operation is now complete and ready for the next phase of development!
