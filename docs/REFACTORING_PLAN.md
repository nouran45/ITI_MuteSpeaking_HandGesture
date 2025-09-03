# Smart Glove Refactoring & Implementation Plan

## Overview
This document outlines a comprehensive plan for transforming the current single-MPU6050 codebase into a multi-sensor smart glove system that can recognize sign language gestures, as specified in the project overview. The plan addresses necessary refactoring, new functionality, and potential challenges.

## 1. I2C Multiplexer Implementation

### Current Limitations
- Code is designed for only one MPU6050 at address 0x68
- No support for addressing multiple sensors

### Required New Files
- `TCA9548A_Config.h` - Configuration for the I2C multiplexer
- `TCA9548A_Interface.h` - Function declarations for multiplexer
- `TCA9548A_Program.c` - Implementation of multiplexer control

### Functions to Implement
```c
// Initialize the TCA9548A multiplexer
void TCA9548A_voidInit(void);

// Select a specific channel (0-7) on the multiplexer
void TCA9548A_voidSelectChannel(u8 u8ChannelId);

// Disable all channels (useful between operations)
void TCA9548A_voidDisableAllChannels(void);

// Get current active channel
u8 TCA9548A_u8GetActiveChannel(void);

// Read from a specific sensor on a specific channel
u8 TCA9548A_u8ReadRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr);

// Write to a specific sensor on a specific channel
void TCA9548A_voidWriteRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8 u8Data);
```

## 2. MPU6050 Module Refactoring

### Functions to Refactor
```c
// Change from:
void MPU6050_voidInit(void);
// To:
void MPU6050_voidInitSensor(u8 u8SensorId);
void MPU6050_voidInitAllSensors(void);

// Change from:
void MPU6050_voidReadAccel(s16* s16AccelX, s16* s16AccelY, s16* s16AccelZ);
// To:
void MPU6050_voidReadAccel(u8 u8SensorId, s16* s16AccelX, s16* s16AccelY, s16* s16AccelZ);

// Change from:
void MPU6050_voidReadGyro(s16* s16GyroX, s16* s16GyroY, s16* s16GyroZ);
// To:
void MPU6050_voidReadGyro(u8 u8SensorId, s16* s16GyroX, s16* s16GyroY, s16* s16GyroZ);

// Private function to refactor:
static u8 MPU6050_u8ReadRegister(u8 u8Reg);
// To:
static u8 MPU6050_u8ReadRegister(u8 u8SensorId, u8 u8Reg);

// Private function to refactor:
static void MPU6050_voidWriteRegister(u8 u8Reg, u8 u8Data);
// To:
static void MPU6050_voidWriteRegister(u8 u8SensorId, u8 u8Reg, u8 u8Data);
```

### New MPU6050 Functions Needed
```c
// New functions for angle calculation
void MPU6050_voidCalculateAngles(u8 u8SensorId, float* fRoll, float* fPitch);

// Read all sensors at once to form a feature vector
void MPU6050_voidReadAllSensorsAngles(float* fRollVector, float* fPitchVector);

// Calibration function
void MPU6050_voidCalibrateSensor(u8 u8SensorId);
void MPU6050_voidCalibrateAllSensors(void);
```

## 3. Data Processing & Feature Extraction

### New Files
- `FeatureExtraction_Config.h` - Configuration for feature extraction
- `FeatureExtraction_Interface.h` - Function declarations
- `FeatureExtraction_Program.c` - Implementation of feature extraction algorithms

### Functions to Implement
```c
// Initialize feature extraction module
void FeatureExtraction_voidInit(void);

// Apply complementary filter to raw IMU data
void FeatureExtraction_voidApplyComplementaryFilter(
    s16 s16AccelX, s16 s16AccelY, s16 s16AccelZ,
    s16 s16GyroX, s16 s16GyroY, s16 s16GyroZ,
    float* fRoll, float* fPitch);

// Create a feature vector from all sensors
void FeatureExtraction_voidCreateFeatureVector(
    float* fRollVector, float* fPitchVector, 
    float* fFeatureVector);

// Normalize feature vector (important for ML)
void FeatureExtraction_voidNormalizeFeatureVector(
    float* fFeatureVector, float* fNormalizedVector);
```

## 4. UART Communication Enhancement

### Functions to Refactor
```c
// Change from:
void UART_voidSendNumber(s16 number);
// To:
void UART_voidSendNumber(s16 number);
void UART_voidSendFloat(float fNumber, u8 u8Precision);
```

### New UART Functions
```c
// Send feature vector to ESP32
void UART_voidSendFeatureVector(float* fFeatureVector, u8 u8VectorSize);

// Send data in a specific format for web dashboard
void UART_voidSendJSONData(float* fRollVector, float* fPitchVector, u8* u8GestureName);

// Receive commands from ESP32
u8 UART_u8ReceiveCommand(void);

// Initialize second UART for ESP32 communication
void UART2_voidInit(void);
void UART2_voidSendByte(u8 u8Data);
u8 UART2_u8ReceiveByte(void);
```

## 5. Gesture Recognition System Integration

### New Files
- `GestureRecognition_Config.h` - Configuration for the gesture system
- `GestureRecognition_Interface.h` - Function declarations
- `GestureRecognition_Program.c` - Integration with ESP32 and feature handling

### Functions to Implement
```c
// Initialize the gesture recognition system
void GestureRecognition_voidInit(void);

// Send feature vector to ESP32 for processing
void GestureRecognition_voidSendFeatureVector(float* fFeatureVector);

// Receive recognized gesture from ESP32
u8 GestureRecognition_u8GetRecognizedGesture(u8* u8GestureName);

// Calibration mode to capture new gestures
void GestureRecognition_voidEnterCalibrationMode(void);
void GestureRecognition_voidCaptureGesture(u8* u8GestureName);
void GestureRecognition_voidExitCalibrationMode(void);
```

## 6. User Interface Enhancements

### New Files
- `UI_Config.h` - Configuration for UI elements
- `UI_Interface.h` - Function declarations for UI
- `UI_Program.c` - Implementation of UI functions

### Functions to Implement
```c
// Initialize UI elements
void UI_voidInit(void);

// Display recognized gesture on LCD
void UI_voidDisplayRecognizedGesture(u8* u8GestureName);

// Display system status
void UI_voidDisplaySystemStatus(void);

// Display calibration instructions
void UI_voidDisplayCalibrationInstructions(u8 u8Step);

// Show sensor readings (debug mode)
void UI_voidDisplaySensorReadings(float* fRollVector, float* fPitchVector);
```

## 7. System Integration

### Main Program Refactoring
The current `main.c` needs significant refactoring to:
1. Initialize all components (5 MPU6050s via multiplexer, ESP32 comms)
2. Implement a state machine for different operational modes
3. Handle timing for multi-sensor sampling
4. Support calibration and training modes

### Main Program States
```c
typedef enum {
    SYSTEM_STATE_INIT,
    SYSTEM_STATE_CALIBRATION,
    SYSTEM_STATE_TRAINING,
    SYSTEM_STATE_RECOGNITION,
    SYSTEM_STATE_DEBUG,
    SYSTEM_STATE_ERROR
} SystemState_t;
```

## 8. Web Dashboard Integration

### New Files
- `WebDashboard_Config.h` - Configuration for web communication
- `WebDashboard_Interface.h` - Function declarations
- `WebDashboard_Program.c` - Implementation of web dashboard communication

### Functions to Implement
```c
// Format data for web dashboard
void WebDashboard_voidFormatJSONData(
    float* fRollVector, float* fPitchVector, 
    u8* u8GestureName, u8* u8JSONBuffer);

// Send data to ESP32 for websocket transmission
void WebDashboard_voidSendData(u8* u8JSONBuffer);
```

## 9. Timing and Performance Optimization

### Critical Timing Considerations
1. Sampling all 5 MPU6050s must be fast enough (aim for <50ms total)
2. I2C multiplexer switching adds overhead
3. Feature extraction calculations are CPU-intensive
4. UART communication to ESP32 must not bottleneck the system

### Timing Optimization Functions
```c
// Optimize I2C read for bulk data
void I2C_voidFastReadMultipleRegisters(u8 u8DevAddr, u8 u8StartReg, u8* u8DataBuffer, u8 u8Length);

// Burst read all sensors in sequence
void MPU6050_voidBurstReadAllSensors(s16* s16AccelData, s16* s16GyroData);
```

## 10. Potential Challenges & Mitigations

### Hardware Challenges
1. **I2C Bus Loading**: With 5 sensors + multiplexer, signal integrity might suffer
   - *Mitigation*: Use appropriate pull-up resistors (2.2kΩ-4.7kΩ), keep I2C traces short

2. **Power Consumption**: Multiple sensors increase power draw
   - *Mitigation*: Implement low-power modes, sensor sleep when not in use

3. **Physical Mounting**: Sensor placement affects angle calculation
   - *Mitigation*: Design proper calibration procedure to account for mounting offsets

### Software Challenges
1. **Floating-Point Calculations**: ATmega32 has no FPU, making angle calculations slow
   - *Mitigation*: Use fixed-point math or lookup tables where possible

2. **Memory Constraints**: Feature vectors and buffers consume RAM
   - *Mitigation*: Optimize data structures, use static allocation

3. **I2C Bus Contention**: Risk of communication failures
   - *Mitigation*: Implement robust error handling and bus recovery

4. **Sensor Drift**: MPU6050 gyros drift over time
   - *Mitigation*: Implement periodic recalibration or zero-motion detection

## 11. Testing Strategy

### Test Cases to Develop
1. Individual sensor verification (read correct values)
2. Multiplexer channel switching validation
3. Feature extraction accuracy test
4. ESP32 communication reliability test
5. End-to-end recognition timing test
6. Gesture recognition accuracy test

### Test Functions
```c
// Verify all sensors are functioning
void Test_voidVerifySensors(void);

// Test feature extraction with known input
void Test_voidFeatureExtraction(void);

// Measure timing performance
void Test_voidMeasureTimingPerformance(void);
```

## 12. Implementation Roadmap

### Phase 1: Hardware & Core Functionality
1. Implement TCA9548A multiplexer interface
2. Refactor MPU6050 functions for multi-sensor support
3. Test basic multi-sensor reading

### Phase 2: Feature Extraction & Communication
1. Implement complementary filter for angle calculation
2. Build feature vector creation functions
3. Enhance UART communication for ESP32 interface
4. Test feature extraction accuracy

### Phase 3: Integration & UI
1. Integrate with ESP32 for gesture recognition
2. Implement UI for gesture display and feedback
3. Add calibration and training modes
4. Test end-to-end system functionality

### Phase 4: Optimization & Dashboard
1. Optimize timing and performance
2. Implement web dashboard communication
3. Refine gesture recognition pipeline
4. Final system testing and validation

## 13. Documentation Needs

1. Updated pin mappings for all sensors and multiplexer
2. Calibration procedure documentation
3. ESP32 communication protocol specification
4. Web dashboard API documentation
5. User manual for gesture training

---

## Summary of Key Files to Create/Modify

### New Files
1. `TCA9548A_Config.h`, `TCA9548A_Interface.h`, `TCA9548A_Program.c`
2. `FeatureExtraction_Config.h`, `FeatureExtraction_Interface.h`, `FeatureExtraction_Program.c`
3. `GestureRecognition_Config.h`, `GestureRecognition_Interface.h`, `GestureRecognition_Program.c`
4. `UI_Config.h`, `UI_Interface.h`, `UI_Program.c`
5. `WebDashboard_Config.h`, `WebDashboard_Interface.h`, `WebDashboard_Program.c`

### Files to Refactor
1. `MPU6050_Interface.h`, `MPU6050_Config.h`, `MPU6050_Program.c`
2. `I2C_Interface.h`, `I2C_Program.c`
3. `UART_Interface.h`, `UART_Program.c`
4. `LCD_Interface.h`, `LCD_Program.c` (for enhanced UI)
5. `main.c` (complete rewrite for state machine)

This plan provides a structured approach to transforming your single-sensor system into a full-featured smart glove with sign language recognition capabilities.
