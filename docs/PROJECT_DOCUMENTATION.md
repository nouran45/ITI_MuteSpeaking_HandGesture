# IMU Testing LCD Project Documentation

## Overview

This project implements a comprehensive IMU (Inertial Measurement Unit) monitoring system using an ATmega32a microcontroller. The system reads data from an MPU6050 accelerometer/gyroscope sensor, displays motion information on an LCD, and streams live data to a PC via UART for data logging and analysis.

## Project Structure

### Hardware Components
- **ATmega32a Microcontroller** (8MHz external crystal)
- **MPU6050 IMU Sensor** (communicates via I2C)
- **16x2 Character LCD** (4-bit mode to conserve pins)
- **USB-to-Serial Adapter** (for PC communication)

### Pin Configuration
```
ATmega32a Pin Mapping:
├── I2C Interface (MPU6050)
│   ├── PC0 - SDA (Serial Data)
│   └── PC1 - SCL (Serial Clock)
├── LCD Interface (4-bit mode)
│   ├── PA0 - RS (Register Select)
│   ├── PA1 - E (Enable)
│   ├── PD4 - D4 (Data bit 4)
│   ├── PD5 - D5 (Data bit 5)
│   ├── PD6 - D6 (Data bit 6)
│   └── PD7 - D7 (Data bit 7)
└── UART Interface
    ├── PD0 - RXD (Receive Data)
    └── PD1 - TXD (Transmit Data)
```

## File Structure Documentation

### Core System Files

#### **STD_TYPES.h**
**Purpose**: Standard type definitions for consistent data types across the project.

**Contents**:
```c
typedef unsigned char u8;    // 8-bit unsigned integer
typedef signed char s8;      // 8-bit signed integer
typedef unsigned short u16;  // 16-bit unsigned integer
typedef signed short s16;    // 16-bit signed integer
typedef unsigned long u32;   // 32-bit unsigned integer
typedef signed long s32;     // 32-bit signed integer
```

**Usage**: Included in all modules to ensure consistent data type usage.

---

#### **BIT_MATH.h**
**Purpose**: Bit manipulation macros for register operations.

**Macros**:
- `SET_BIT(VAR,BITNO)` - Sets a specific bit to 1
- `CLR_BIT(VAR,BITNO)` - Clears a specific bit to 0
- `TOG_BIT(VAR,BITNO)` - Toggles a specific bit
- `GET_BIT(VAR,BITNO)` - Returns the value of a specific bit

**Usage**: Essential for low-level hardware register manipulation.

---

### Digital I/O Module

#### **DIO_Interface.h**
**Purpose**: Digital Input/Output interface definitions and function prototypes.

**Key Definitions**:
- Port identifiers: `DIO_u8_PORTA`, `DIO_u8_PORTB`, `DIO_u8_PORTC`, `DIO_u8_PORTD`
- Pin identifiers: `DIO_u8_PIN0` through `DIO_u8_PIN7`
- Logic levels: `DIO_u8_HIGH`, `DIO_u8_LOW`
- Directions: `DIO_u8_INPUT`, `DIO_u8_OUTPUT`

**Functions**:
- `DIO_voidInit()` - Initializes all ports to safe states
- `DIO_voidSetPinDirection()` - Sets individual pin direction
- `DIO_voidSetPinValue()` - Sets individual pin output value
- `DIO_u8GetPinValue()` - Reads individual pin input value
- `DIO_voidSetPortDirection()` - Sets entire port direction
- `DIO_voidSetPortValue()` - Sets entire port output value

#### **DIO_Register.h**
**Purpose**: Hardware register address definitions for ATmega32a ports.

**Register Mappings**:
```c
PORTA, DDRA, PINA  // Port A registers
PORTB, DDRB, PINB  // Port B registers  
PORTC, DDRC, PINC  // Port C registers
PORTD, DDRD, PIND  // Port D registers
```

#### **DIO_PROGRAM.c**
**Purpose**: Implementation of digital I/O functions.

**Key Features**:
- Safe initialization with all ports set to input/low
- I2C pin protection (PC0/PC1 handled specially)
- Robust error handling for invalid parameters

---

### LCD Display Module

#### **LCD_Interface.h**
**Purpose**: LCD control interface with support for both 4-bit and 8-bit modes.

**Main Functions**:
- `LCD_vidInit_4bits()` - Initialize LCD in 4-bit mode (recommended)
- `LCD_vidSendCommand()` - Send control commands to LCD
- `LCD_vidWriteCharctr()` - Write single character
- `LCD_vidWriteString()` - Write string to LCD
- `LCD_vidGotoxy()` - Set cursor position
- `LCD_vidWriteNumber()` - Display numbers
- `LCD_vidWriteSignedNumber()` - Display signed numbers
- `LCD_Clear()` - Clear display

#### **LCD_Config.h**
**Purpose**: LCD hardware configuration and command definitions.

**Configuration**:
- Pin assignments for control and data lines
- LCD command codes for initialization and operation
- Display modes and options

#### **LCD_Program.c**
**Purpose**: Complete LCD driver implementation.

**Key Features**:
- **4-bit Mode Implementation**: Conserves pins by using only 4 data lines
- **Robust Initialization**: Multiple initialization attempts for reliability
- **Pin Optimization**: Uses PA0/PA1 for control to free PC0/PC1 for I2C
- **Timing Compliance**: Proper delays for LCD command processing

**Technical Details**:
```c
// 4-bit initialization sequence
1. Send 0x30 three times (8-bit mode attention)
2. Send 0x20 (switch to 4-bit mode)
3. Send 0x28 (4-bit, 2 lines, 5x8 font)
4. Send 0x08 (display off)
5. Send 0x01 (clear display)
6. Send 0x06 (entry mode)
7. Send 0x0C (display on)
```

---

### I2C Communication Module

#### **I2C_Interface.h**
**Purpose**: I2C (TWI) communication interface for sensor communication.

**Functions**:
- `I2C_voidInit()` - Initialize I2C hardware
- `I2C_voidStart()` - Generate START condition
- `I2C_voidStop()` - Generate STOP condition
- `I2C_voidWrite()` - Write byte to I2C bus
- `I2C_u8ReadWithACK()` - Read byte with acknowledgment
- `I2C_u8ReadWithNACK()` - Read byte with no acknowledgment
- `I2C_u8GetStatus()` - Get current I2C status

#### **I2C_Config.h**
**Purpose**: I2C configuration parameters and status codes.

**Configuration**:
- Standard I2C status codes for error checking
- Prescaler options for different clock speeds
- Bit rate calculation for 100kHz operation

#### **I2C_Program.c**
**Purpose**: I2C hardware driver implementation.

**Key Features**:
- **100kHz Operation**: Standard I2C speed for sensor communication
- **Timeout Protection**: Prevents system hanging on I2C errors
- **Status Monitoring**: Comprehensive error detection
- **ATmega32a Compatibility**: Correct register addresses for TWI

**Technical Details**:
```c
// I2C Clock Calculation
TWBR = ((F_CPU/SCL_CLOCK)-16)/(2*PRESCALER)
// For 100kHz: TWBR = ((8000000/100000)-16)/(2*1) = 32
```

---

### MPU6050 Sensor Module

#### **MPU6050_Interface.h**
**Purpose**: Interface for MPU6050 accelerometer/gyroscope sensor.

**Functions**:
- `MPU6050_voidInit()` - Initialize MPU6050 sensor
- `MPU6050_voidReadAccel()` - Read accelerometer data (X,Y,Z)
- `MPU6050_voidReadGyro()` - Read gyroscope data (X,Y,Z)
- `MPU6050_voidReadTemp()` - Read temperature sensor

#### **MPU6050_Config.h**
**Purpose**: MPU6050 register addresses and configuration values.

**Key Registers**:
- Power management, sample rate, and sensor configuration
- Accelerometer and gyroscope data registers
- Temperature sensor registers

#### **MPU6050_Program.c**
**Purpose**: Complete MPU6050 driver implementation.

**Key Features**:
- **Sensor Initialization**: Proper wake-up and configuration sequence
- **Multi-byte Reads**: Efficient reading of 16-bit sensor values
- **Configurable Ranges**: ±2g accelerometer, ±250°/s gyroscope
- **Low-pass Filtering**: Digital filtering for noise reduction

**Data Processing**:
```c
// Motion Detection Thresholds
Tilt Detection: |AccelX/Y| > 5000
Face Up/Down: AccelZ > ±12000
Rotation: |GyroZ| > 3000
```

---

### UART Communication Module

#### **UART_Interface.h**
**Purpose**: Serial communication interface for PC connectivity.

**Functions**:
- `UART_voidInit()` - Initialize UART at 9600 baud
- `UART_voidSendByte()` - Send single byte
- `UART_voidSendString()` - Send text string
- `UART_voidSendNumber()` - Send numeric data

#### **UART_Program.c**
**Purpose**: UART driver implementation for data streaming.

**Key Features**:
- **9600 Baud Rate**: Standard speed for reliable communication
- **8-N-1 Format**: 8 data bits, no parity, 1 stop bit
- **Number Conversion**: Automatic integer to ASCII conversion
- **Streaming Optimized**: Efficient for continuous data transmission

---

### Main Application

#### **main.c**
**Purpose**: Main application integrating all modules for IMU monitoring.

**System Flow**:
```
1. Initialize all peripherals (DIO, UART, LCD, I2C, MPU6050)
2. Display initialization progress on LCD
3. Send startup message to PC
4. Enter main loop:
   a. Read sensor data from MPU6050
   b. Process motion detection
   c. Stream CSV data to PC
   d. Update LCD display
   e. Delay 200ms for live updates
```

**Data Output Format**:
```
CSV Format: AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ,Motion_Status
Example: 1234,-567,9876,123,-45,789,TILT_RIGHT_SPIN_LEFT
```

**Motion Detection Logic**:
- **Tilt Detection**: Monitors X/Y acceleration for device orientation
- **Face Orientation**: Uses Z acceleration to detect face up/down
- **Rotation Detection**: Uses Z gyroscope for spinning motion
- **Combined States**: Reports multiple simultaneous motions

---

### Diagnostic and Test Files

#### **LCD_diagnostic_tool.c**
**Purpose**: Standalone LCD testing program for hardware verification.

**Features**:
- Manual LCD initialization sequence
- Hardware-level pin control
- Test pattern display ("LCD TEST" and "12345678")
- Blinking cursor to verify operation

**Usage**: Compile separately to test LCD functionality in isolation.

#### **lcd_test_only.c**
**Purpose**: Simple LCD counter test for basic functionality verification.

**Features**:
- Counter display updating every 100ms
- Tests LCD number display functions
- Verifies LCD timing and character writing

---

### PC Interface Scripts

#### **mpu6050_receiver.py**
**Purpose**: Python script for receiving and logging sensor data from the microcontroller.

**Features**:
- **Real-time Data Display**: Shows live sensor readings in terminal
- **CSV Logging**: Automatic data logging with timestamps
- **Error Handling**: Robust serial communication with error recovery
- **Cross-platform**: Works on Linux, Windows, and macOS

**Output**:
- Creates timestamped CSV files: `mpu6050_data_YYYYMMDD_HHMMSS.csv`
- Displays formatted data: `[HH:MM:SS.mmm] AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ,Motion`

#### **serial_monitor.sh**
**Purpose**: Bash script for simple serial monitoring.

**Features**:
- Automatic port detection
- Configurable baud rate
- Uses `screen` or `stty/cat` for serial communication
- Error handling for missing ports

**Usage**:
```bash
./serial_monitor.sh [port] [baudrate]
./serial_monitor.sh /dev/ttyUSB0 9600
```

---

### Build System

#### **Makefile**
**Purpose**: Automated build system for compiling and flashing the project.

**Configuration**:
```makefile
MCU=atmega32a        # Target microcontroller
F_CPU=8000000UL      # CPU frequency
CC=avr-gcc           # Compiler
CFLAGS=-Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)  # Optimization flags
```

**Targets**:
- `make` or `make all` - Compile to ELF and HEX files
- `make flash` - Program the microcontroller via USBasp
- `make clean` - Remove compiled files

**Build Process**:
```
1. Compile all .c files to .o object files
2. Link object files to create .elf executable
3. Convert .elf to .hex file for programming
4. Optional: Flash .hex to microcontroller
```

---

## System Operation

### Startup Sequence
1. **Hardware Initialization**: All peripherals initialized in sequence
2. **Progress Display**: LCD shows initialization status for each module
3. **Sensor Wake-up**: MPU6050 powered up and configured
4. **Communication Setup**: UART established for PC link
5. **Ready State**: System begins continuous operation

### Runtime Operation
1. **Sensor Reading**: MPU6050 data read via I2C every 200ms
2. **Motion Analysis**: Acceleration and gyroscope data processed for motion detection
3. **LCD Update**: Current sensor values displayed on LCD (simplified view)
4. **PC Streaming**: Complete sensor data streamed to PC in CSV format
5. **Data Logging**: PC script logs all data with timestamps

### Data Flow
```
MPU6050 → I2C → ATmega32a → Processing → LCD Display
                     ↓
                   UART → PC → Python Script → CSV File
```

---

## Technical Specifications

### Communication Protocols
- **I2C**: 100kHz, 7-bit addressing, MPU6050 address 0x68
- **UART**: 9600 baud, 8-N-1 format, TTL levels
- **LCD**: 4-bit parallel interface, HD44780 compatible

### Timing Requirements
- **Main Loop**: 200ms cycle time for live updates
- **LCD Commands**: 1-2ms delay per command
- **I2C Transactions**: ~1ms per sensor reading
- **UART Transmission**: ~10ms per data packet

### Memory Usage
- **Flash**: ~8KB for complete application
- **RAM**: ~512 bytes for variables and stack
- **EEPROM**: Not used in current implementation

---

## Troubleshooting Guide

### Common Issues

#### LCD Not Displaying
1. Check power connections (VCC, GND)
2. Verify contrast adjustment (V0 pin)
3. Confirm pin connections match configuration
4. Use LCD diagnostic tool for hardware test

#### I2C Communication Errors
1. Verify pull-up resistors on SDA/SCL lines (4.7kΩ recommended)
2. Check MPU6050 power supply (3.3V or 5V depending on module)
3. Confirm I2C address (0x68 is default, some modules use 0x69)
4. Ensure no other I2C devices conflict

#### UART Communication Problems
1. Verify baud rate matches (9600)
2. Check TX/RX connections (may need crossover)
3. Confirm ground connection between devices
4. Test with simple terminal program

#### No Sensor Data
1. Check MPU6050 initialization sequence
2. Verify I2C communication is working
3. Ensure sensor is not in sleep mode
4. Check for proper power supply decoupling

---

## Expansion Possibilities

### Hardware Enhancements
- Add SD card logging for standalone operation
- Include RTC module for accurate timestamps
- Add wireless communication (WiFi/Bluetooth)
- Include additional sensors (magnetometer, pressure)

### Software Improvements
- Implement Kalman filtering for better sensor fusion
- Add calibration routines for sensor accuracy
- Include power management for battery operation
- Develop real-time motion recognition algorithms

### Data Analysis Features
- Implement FFT for frequency domain analysis
- Add gesture recognition capabilities
- Include activity classification (walking, running, etc.)
- Develop anomaly detection algorithms

---

## Version History

### Current Version: 1.0
- Complete IMU monitoring system
- LCD display with 4-bit interface
- PC data streaming via UART
- Real-time motion detection
- Python data logging script

### Future Versions
- Enhanced motion algorithms
- Wireless connectivity
- Mobile app interface
- Machine learning integration

---

## Conclusion

This project demonstrates a complete embedded systems implementation combining multiple communication protocols, sensor interfacing, and real-time data processing. The modular design allows for easy expansion and modification while maintaining robust operation across various conditions.

The system successfully bridges the gap between embedded sensor hardware and PC-based data analysis, making it suitable for educational purposes, prototyping, and development of more complex motion sensing applications.
