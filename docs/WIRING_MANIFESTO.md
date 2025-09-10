# Smart Glove Wiring Manifesto

This document provides detailed pinout and connection information for the Smart Glove project, including all connections between the ATmega32a microcontroller, MPU6050 sensors, TCA9548A I2C multiplexer, LCD display, and ESP32.

## ATmega32a Pinout

### Port A (PA0-PA7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PA0 | LCD Register Select | LCD pin RS                 | Control pin for LCD                  |
| PA1 | LCD Enable        | LCD pin E                   | Control pin for LCD                  |
| PA2 | Unused            | -                           | Available for future use             |
| PA3 | Unused            | -                           | Available for future use             |
| PA4 | Unused            | -                           | Available for future use             |
| PA5 | Unused            | -                           | Available for future use             |
| PA6 | Unused            | -                           | Available for future use             |
| PA7 | Unused            | -                           | Available for future use             |

### Port B (PB0-PB7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PB0 | Unused            | -                           | Available for future use             |
| PB1 | Unused            | -                           | Available for future use             |
| PB2 | Unused            | -                           | Available for future use             |
| PB3 | MOSI (ISP)        | ISP Programmer              | For programming                      |
| PB4 | MISO (ISP)        | ISP Programmer              | For programming                      |
| PB5 | SCK (ISP)         | ISP Programmer              | For programming                      |
| PB6 | Unused            | -                           | Available for future use             |
| PB7 | Unused            | -                           | Available for future use             |

### Port C (PC0-PC7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PC0 | I2C SDA           | TCA9548A SDA                | I2C Data line (4.7kΩ pull-up)        |
| PC1 | I2C SCL           | TCA9548A SCL                | I2C Clock line (4.7kΩ pull-up)       |
| PC2 | Unused            | -                           | Available for future use             |
| PC3 | Unused            | -                           | Available for future use             |
| PC4 | Unused            | -                           | Available for future use             |
| PC5 | Unused            | -                           | Available for future use             |
| PC6 | Unused            | -                           | Available for future use             |
| PC7 | Unused            | -                           | Available for future use             |

### Port D (PD0-PD7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PD0 | UART RX           | PC/Serial Adapter TX        | Serial communication to PC           |
| PD1 | UART TX           | PC/Serial Adapter RX        | Serial communication to PC           |
| PD2 | Unused            | -                           | Available for future use             |
| PD3 | Unused            | -                           | Available for future use             |
| PD4 | LCD Data Bit 4    | LCD pin D4                  | Data pin for LCD in 4-bit mode      |
| PD5 | LCD Data Bit 5    | LCD pin D5                  | Data pin for LCD in 4-bit mode      |
| PD6 | LCD Data Bit 6    | LCD pin D6                  | Data pin for LCD in 4-bit mode      |
| PD7 | LCD Data Bit 7    | LCD pin D7                  | Data pin for LCD in 4-bit mode      |

## TCA9548A I2C Multiplexer

| Pin      | Connected To                 | Notes                                |
|----------|------------------------------|--------------------------------------|
| VCC      | 3.3V Power Supply            | Power supply for multiplexer         |
| GND      | GND                          | Ground connection                    |
| SCL      | ATmega32a PC1               | I2C Clock line                       |
| SDA      | ATmega32a PC0               | I2C Data line                        |
| RESET    | VCC                         | Reset pin tied HIGH (normal operation)|
| A0       | GND                         | Address bit 0 = 0                    |
| A1       | GND                         | Address bit 1 = 0                    |
| A2       | GND                         | Address bit 2 = 0                    |
| SD0-SD7  | MPU6050 SDA (×5)            | I2C Data lines for each channel      |
| SC0-SC7  | MPU6050 SCL (×5)            | I2C Clock lines for each channel     |

## MPU6050 Sensors (×5)

### Sensor Placement
- Sensor 0: Thumb (Channel 0 on multiplexer)
- Sensor 1: Index Finger (Channel 1 on multiplexer)
- Sensor 2: Middle Finger (Channel 2 on multiplexer)
- Sensor 3: Ring Finger (Channel 3 on multiplexer)
- Sensor 4: Palm/Hand Orientation (Channel 4 on multiplexer)

### Connections (Same for all 5 sensors)
| Pin      | Connected To                 | Notes                                |
|----------|------------------------------|--------------------------------------|
| VCC      | 3.3V Power Supply            | Power supply for MPU6050             |
| GND      | GND                          | Ground connection                    |
| SCL      | TCA9548A SCx                | I2C Clock line from multiplexer      |
| SDA      | TCA9548A SDx                | I2C Data line from multiplexer       |
| XDA      | Not connected                | Not used in current implementation   |
| XCL      | Not connected                | Not used in current implementation   |
| AD0      | GND                          | Sets I2C address to 0x68             |
| INT      | Not connected                | Not used in current implementation   |

## LCD Display (16x2 Character LCD)

| Pin      | Connected To                 | Notes                                |
|----------|------------------------------|--------------------------------------|
| VSS      | GND                          | Ground connection                    |
| VDD      | 5V Power Supply              | Power supply for LCD                 |
| V0       | 10kΩ Potentiometer           | Contrast adjustment                  |
| RS       | ATmega32a PA0                | Register Select                      |
| RW       | GND                          | Always in write mode                 |
| E        | ATmega32a PA1                | Enable signal                        |
| D0       | Not connected                | Not used in 4-bit mode               |
| D1       | Not connected                | Not used in 4-bit mode               |
| D2       | Not connected                | Not used in 4-bit mode               |
| D3       | Not connected                | Not used in 4-bit mode               |
| D4       | ATmega32a PD4                | Data bit 4                           |
| D5       | ATmega32a PD5                | Data bit 5                           |
| D6       | ATmega32a PD6                | Data bit 6                           |
| D7       | ATmega32a PD7                | Data bit 7                           |
| A        | 5V (via 220Ω resistor)       | Backlight anode                      |
| K        | GND                          | Backlight cathode                    |

## Power Supply

| Connection | Description                          | Notes                             |
|------------|--------------------------------------|-----------------------------------|
| VCC (5V)   | 5V from power supply or regulator    | For ATmega32a and LCD             |
| 3.3V       | 3.3V regulator output               | For MPU6050 and TCA9548A          |
| GND        | Common ground                        | All components                    |

## Pull-up Resistors

| Resistor   | Connected Between                    | Notes                             |
|------------|--------------------------------------|-----------------------------------|
| 4.7kΩ      | SDA line and 3.3V                   | I2C pull-up (required)            |
| 4.7kΩ      | SCL line and 3.3V                   | I2C pull-up (required)            |

## Additional Components

| Component           | Connected Between                    | Notes                             |
|---------------------|--------------------------------------|-----------------------------------|
| 16MHz Crystal       | ATmega32a XTAL1 and XTAL2            | Clock source                      |
| 22pF Capacitors (2) | Each crystal pin to GND              | Crystal load capacitors           |
| 10kΩ Potentiometer  | 5V, LCD V0, and GND                  | LCD contrast adjustment           |
| 100nF Capacitors    | VCC to GND near each IC              | Decoupling capacitors            |
| 10μF Capacitor      | VCC to GND                           | Bulk decoupling                  |

## Assembly Notes

1. **I2C Pull-up Resistors**: The I2C bus requires pull-up resistors on both SDA and SCL lines. Use 4.7kΩ resistors connected to 3.3V.

2. **Power Supply Considerations**: The system uses both 5V (for ATmega32a and LCD) and 3.3V (for MPU6050 sensors and TCA9548A). Ensure proper voltage regulation.

3. **Sensor Placement**: Mount the MPU6050 sensors securely on each finger joint and on the back of the hand. Use flexible wiring to allow for movement.

4. **Cable Management**: Use flexible, lightweight cabling for connections to finger-mounted sensors. Consider using ribbon cables or flexible PCB for better comfort and durability.

5. **Decoupling Capacitors**: Place 100nF decoupling capacitors as close as possible to the power pins of each IC to reduce noise.

6. **Ground Plane**: Implement a solid ground plane to minimize noise and ensure proper operation of the I2C bus and sensors.

7. **UART Communication**: The UART communication to PC operates at 9600 baud by default for sensor data streaming.

8. **Reset Circuit**: The ATmega32a should have a proper reset circuit with a pull-up resistor and optional reset button.

9. **ISP Programming Header**: Include a standard 6-pin or 10-pin ISP header for programming the ATmega32a.

10. **Sensor Calibration**: After assembly, the system may need calibration to account for mounting variations and sensor drift.

---

## Communication Architecture Overview

### I2C Bus Usage
**Primary Purpose**: Multi-sensor communication via TCA9548A multiplexer

#### I2C Configuration:
- **SDA Pin**: PC0 (ATmega32a) ↔ TCA9548A SDA
- **SCL Pin**: PC1 (ATmega32a) ↔ TCA9548A SCL  
- **Speed**: 100kHz (standard mode)
- **Pull-up Resistors**: 4.7kΩ on both SDA and SCL lines
- **Protocol**: 7-bit addressing

#### I2C Device Addresses:
- **TCA9548A Multiplexer**: 0x70 (default address)
- **MPU6050 Sensors**: 0x68 (all 5 sensors, selected via multiplexer channels)

#### I2C Communication Flow:
```
ATmega32 ←I2C→ TCA9548A ←Channel 0→ [Thumb MPU6050]
                 ↓      ←Channel 1→ [Index MPU6050] 
                 ↓      ←Channel 2→ [Middle MPU6050]
                 ↓      ←Channel 3→ [Ring MPU6050]
                 ↓      ←Channel 4→ [Palm MPU6050]
```

### UART Communication Usage  
**Primary Purpose**: Real-time data streaming to PC/laptop

#### UART Configuration:
- **TX Pin**: PD1 (ATmega32a) → PC/Serial Adapter RX
- **RX Pin**: PD0 (ATmega32a) ← PC/Serial Adapter TX
- **Baud Rate**: 9600 bps
- **Format**: 8-N-1 (8 data bits, no parity, 1 stop bit)
- **Voltage**: TTL levels (0V-5V)

#### UART Data Stream Format:
```
S0:AX=1234,AY=5678,AZ=9012,GX=3456,GY=7890,GZ=1234;S1:AX=2345,AY=6789...
```
Where:
- **S0-S4**: Sensor IDs (0=Thumb, 1=Index, 2=Middle, 3=Ring, 4=Palm)
- **AX,AY,AZ**: Accelerometer X,Y,Z values (raw 16-bit)
- **GX,GY,GZ**: Gyroscope X,Y,Z values (raw 16-bit)

### LCD Interface (4-Bit Mode)
**Purpose**: Local status display and system monitoring

#### LCD Pin Configuration (HD44780 Compatible):
- **RS (Register Select)**: PA0
- **E (Enable)**: PA1  
- **Data Lines**: PD4, PD5, PD6, PD7 (4-bit mode)
- **Power**: 5V VCC, GND
- **Contrast**: 10kΩ potentiometer on V0 pin

#### LCD Display Content:
- **Line 1**: System status messages during boot
- **Line 2**: Real-time sensor data from Sensor 0 (Thumb)
- **Boot Sequence**: "LCD: OK" → "I2C: OK" → "MUX: OK" → "MPU: OK" → "Sensors: 5/5"

### System Power Distribution
**Total Power Requirements**: ~260mA @ 5V

#### Power Consumption Breakdown:
- **ATmega32a**: ~20mA @ 16MHz
- **TCA9548A**: ~5mA  
- **5× MPU6050**: ~3.5mA each (17.5mA total)
- **LCD Display**: ~20mA (with backlight: ~200mA)

#### Power Supply Requirements:
- **Input Voltage**: 7-12V DC (via voltage regulator)
- **Regulated 5V**: For ATmega32a, LCD, and logic levels
- **Regulated 3.3V**: For MPU6050 sensors and TCA9548A
- **Decoupling**: 100nF ceramic + 10μF electrolytic per power rail

---

## Data Flow Architecture

### Real-Time Operation Sequence:
1. **System Boot** (2-3 seconds):
   ```
   Power On → GPIO Init → UART Init → LCD Init → I2C Init → 
   TCA9548A Init → Sensor Detection → Sensor Registration → Ready
   ```

2. **Continuous Data Loop** (200ms cycle):
   ```
   For each sensor (0-4):
     → TCA9548A Select Channel
     → MPU6050 Read Accel/Gyro  
     → Format Data → UART Transmit
   → Update LCD Display → Repeat
   ```

3. **Error Handling**:
   - **I2C Timeout**: Retry communication 3 times
   - **Sensor Disconnection**: Mark sensor as offline, continue with others
   - **LCD Errors**: Continue operation, report via UART
   - **UART Buffer Full**: Implement flow control

### Performance Specifications:
- **Sensor Sampling Rate**: ~5Hz per sensor (limited by 200ms main loop)
- **Total Data Throughput**: ~25 sensor readings/second
- **UART Bandwidth Usage**: ~2400 bps (25% of available 9600 bps)
- **I2C Bus Utilization**: ~15% (plenty of headroom for expansion)
- **System Latency**: <50ms from sensor reading to UART output

---

## Troubleshooting Guide

### Common Issues and Solutions:

#### I2C Communication Problems:
- **Check Pull-up Resistors**: Ensure 4.7kΩ on SDA/SCL
- **Verify Connections**: PC0=SDA, PC1=SCL  
- **Power Supply**: Ensure stable 3.3V to sensors
- **Bus Speed**: Reduce to 50kHz if communication fails

#### UART Data Issues:
- **Baud Rate Mismatch**: Verify 9600 bps on both ends
- **Wiring**: Connect ATmega32a TX (PD1) to PC serial adapter RX
- **Ground Reference**: Common ground between ATmega32a and PC

#### Sensor Detection Failures:
- **WHO_AM_I Check**: Should return 0x68 for MPU6050
- **Channel Selection**: Verify TCA9548A channel switching
- **Power Sequencing**: Power sensors after TCA9548A initialization

#### LCD Display Problems:
- **Contrast Adjustment**: Use potentiometer to set visibility
- **4-bit Wiring**: Verify PA0/PA1 for control, PD4-PD7 for data
- **Timing**: Ensure proper command delays (1-2ms)
