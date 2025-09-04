# Smart Glove Wiring Manifesto

This document provides detailed pinout and connection information for the Smart Glove project, including all connections between the ATmega32a microcontroller, MPU6050 sensors, TCA9548A I2C multiplexer, LCD display, and ESP32.

## ATmega32a Pinout

### Port A (PA0-PA7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PA0 | LCD Register Select | LCD pin RS                 | Control pin for LCD                  |
| PA1 | LCD Enable        | LCD pin E                   | Control pin for LCD                  |
| PA2 | LCD Data Bit 4    | LCD pin D4                  | Data pin for LCD in 4-bit mode       |
| PA3 | LCD Data Bit 5    | LCD pin D5                  | Data pin for LCD in 4-bit mode       |
| PA4 | LCD Data Bit 6    | LCD pin D6                  | Data pin for LCD in 4-bit mode       |
| PA5 | LCD Data Bit 7    | LCD pin D7                  | Data pin for LCD in 4-bit mode       |
| PA6 | Button Input      | Calibration Button          | External pull-up resistor (10kΩ)     |
| PA7 | Button Input      | Mode Selection Button       | External pull-up resistor (10kΩ)     |

### Port B (PB0-PB7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PB0 | Multiplexer Reset | TCA9548A Reset pin          | Active LOW                           |
| PB1 | ESP32 Control     | ESP32 GPIO 26               | Communication control                |
| PB2 | INT Input         | MPU6050 INT (via multiplexer)| Interrupt signal from IMU            |
| PB3 | MOSI (ISP)        | ISP Programmer              | For programming                      |
| PB4 | MISO (ISP)        | ISP Programmer              | For programming                      |
| PB5 | SCK (ISP)         | ISP Programmer              | For programming                      |
| PB6 | Unused            | -                           |                                      |
| PB7 | Unused            | -                           |                                      |

### Port C (PC0-PC7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PC0 | I2C SDA           | TCA9548A SDA                | I2C Data line (4.7kΩ pull-up)        |
| PC1 | I2C SCL           | TCA9548A SCL                | I2C Clock line (4.7kΩ pull-up)       |
| PC2 | Status LED Green  | Green LED + Resistor (330Ω) | System status indicator              |
| PC3 | Status LED Red    | Red LED + Resistor (330Ω)   | Error indicator                      |
| PC4 | Status LED Blue   | Blue LED + Resistor (330Ω)  | Activity indicator                   |
| PC5 | ESP32 Handshake   | ESP32 GPIO 27               | Handshaking signal                   |
| PC6 | Unused            | -                           |                                      |
| PC7 | Unused            | -                           |                                      |

### Port D (PD0-PD7)
| Pin | Function          | Connected To                 | Notes                                |
|-----|-------------------|------------------------------|--------------------------------------|
| PD0 | UART RX           | ESP32 TX (GPIO 17)          | Serial communication with ESP32      |
| PD1 | UART TX           | ESP32 RX (GPIO 16)          | Serial communication with ESP32      |
| PD2 | Multiplexer A0    | TCA9548A A0                 | Address bit 0 (for I2C address)      |
| PD3 | Multiplexer A1    | TCA9548A A1                 | Address bit 1 (for I2C address)      |
| PD4 | Multiplexer A2    | TCA9548A A2                 | Address bit 2 (for I2C address)      |
| PD5 | Power LED         | Power LED + Resistor (330Ω) | Power indicator                      |
| PD6 | ESP32 Reset       | ESP32 EN pin                | Reset control for ESP32              |
| PD7 | Buzzer            | Buzzer + Transistor         | Audio feedback                       |

## TCA9548A I2C Multiplexer

| Pin      | Connected To                 | Notes                                |
|----------|------------------------------|--------------------------------------|
| VCC      | 3.3V Power Supply            | Power supply for multiplexer         |
| GND      | GND                          | Ground connection                    |
| SCL      | ATmega32a PC1               | I2C Clock line                       |
| SDA      | ATmega32a PC0               | I2C Data line                        |
| RESET    | ATmega32a PB0               | Reset pin (active LOW)               |
| A0       | ATmega32a PD2               | Address bit 0                        |
| A1       | ATmega32a PD3               | Address bit 1                        |
| A2       | ATmega32a PD4               | Address bit 2                        |
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
| XDA      | Not connected                |                                      |
| XCL      | Not connected                |                                      |
| AD0      | GND                          | Sets I2C address to 0x68             |
| INT      | TCA9548A /INT (Channel-specific) | Interrupt output                 |

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
| D4       | ATmega32a PA2                | Data bit 4                           |
| D5       | ATmega32a PA3                | Data bit 5                           |
| D6       | ATmega32a PA4                | Data bit 6                           |
| D7       | ATmega32a PA5                | Data bit 7                           |
| A        | 5V (via 220Ω resistor)       | Backlight anode                      |
| K        | GND                          | Backlight cathode                    |

## ESP32 Connection

| ESP32 Pin | Connected To                 | Notes                                |
|-----------|------------------------------|--------------------------------------|
| 3.3V      | 3.3V Power Supply            | Power supply for ESP32               |
| GND       | GND                          | Ground connection                    |
| GPIO 16   | ATmega32a PD1 (TX)           | Serial RX on ESP32                   |
| GPIO 17   | ATmega32a PD0 (RX)           | Serial TX on ESP32                   |
| GPIO 26   | ATmega32a PB1                | Control signal                       |
| GPIO 27   | ATmega32a PC5                | Handshaking signal                   |
| EN        | ATmega32a PD6                | Reset control                        |

## Power Supply

| Connection | Description                          | Notes                             |
|------------|--------------------------------------|-----------------------------------|
| VCC (5V)   | 5V from power supply or regulator    | For ATmega32a and LCD             |
| 3.3V       | 3.3V regulator output               | For MPU6050, TCA9548A, and ESP32  |
| GND        | Common ground                        | All components                    |

## Pull-up Resistors

| Resistor   | Connected Between                    | Notes                             |
|------------|--------------------------------------|-----------------------------------|
| 4.7kΩ      | SDA line and 3.3V                   | I2C pull-up                       |
| 4.7kΩ      | SCL line and 3.3V                   | I2C pull-up                       |
| 10kΩ       | PA6 and 5V                          | Button pull-up                    |
| 10kΩ       | PA7 and 5V                          | Button pull-up                    |

## Additional Components

| Component           | Connected Between                    | Notes                             |
|---------------------|--------------------------------------|-----------------------------------|
| 16MHz Crystal       | ATmega32a XTAL1 and XTAL2            | Clock source                      |
| 22pF Capacitors (2) | Each crystal pin to GND              | Crystal load capacitors           |
| 10kΩ Potentiometer  | 5V, LCD V0, and GND                  | LCD contrast adjustment           |
| 100nF Capacitors    | VCC to GND near each IC              | Decoupling capacitors            |
| 10μF Capacitor      | VCC to GND                           | Bulk decoupling                  |
| 330Ω Resistors      | For all LEDs                         | Current limiting                  |
| NPN Transistor      | For buzzer control                   | Current amplification             |

## Assembly Notes

1. **I2C Pull-up Resistors**: The I2C bus requires pull-up resistors on both SDA and SCL lines. Use 4.7kΩ resistors connected to 3.3V.

2. **Power Supply Considerations**: The system uses both 5V (for ATmega32a and LCD) and 3.3V (for MPU6050 sensors, TCA9548A, and ESP32). Ensure proper voltage regulation.

3. **Sensor Placement**: Mount the MPU6050 sensors securely on each finger joint and on the back of the hand. Use flexible wiring to allow for movement.

4. **Cable Management**: Use flexible, lightweight cabling for connections to finger-mounted sensors. Consider using ribbon cables or flexible PCB for better comfort and durability.

5. **Decoupling Capacitors**: Place 100nF decoupling capacitors as close as possible to the power pins of each IC to reduce noise.

6. **Ground Plane**: Implement a solid ground plane to minimize noise and ensure proper operation of the I2C bus and sensors.

7. **ESP32 Communication**: The UART communication between ATmega32a and ESP32 operates at 9600 baud by default. This can be adjusted in the firmware if needed.

8. **Reset Circuit**: The ATmega32a should have a proper reset circuit with a pull-up resistor and optional reset button.

9. **ISP Programming Header**: Include a standard 6-pin or 10-pin ISP header for programming the ATmega32a.

10. **Sensor Calibration**: After assembly, the system will need calibration to account for mounting variations. Use the calibration button (PA6) to trigger the calibration routine.
