# Smart Glove Schematic Diagrams

This document contains ASCII schematic diagrams for the Smart Glove project, showing the key connections between components.

## ATmega32a to LCD Connection

```
                 ATmega32a                  16x2 LCD Display
               +-----------+                +-----------+
           PA0 |           |                |           |
     LCD RS -->|           |--------------->| RS        |
           PA1 |           |                |           |
     LCD E  -->|           |--------------->| E         |
           PA2 |           |                |           |
     LCD D4 -->|           |--------------->| D4        |
           PA3 |           |                |           |
     LCD D5 -->|           |--------------->| D5        |
           PA4 |           |                |           |
     LCD D6 -->|           |--------------->| D6        |
           PA5 |           |                |           |
     LCD D7 -->|           |--------------->| D7        |
               |           |                |           |
               +-----------+                +-----------+
                                                  |
                                                  v
                                             [Contrast Pot]
                                                  |
                                                 GND
```

## ATmega32a to TCA9548A I2C Multiplexer Connection

```
                 ATmega32a                  TCA9548A Multiplexer
               +-----------+                +-----------+
           PC0 |           |                |           |
     I2C SDA -->|           |<------------->| SDA       |
           PC1 |           |                |           |
     I2C SCL -->|           |<------------->| SCL       |
               |           |                |           |
           PB0 |           |                |           |
     RESET   -->|           |--------------->| RESET     |
               |           |                |           |
           PD2 |           |                |           |
     A0      -->|           |--------------->| A0        |
           PD3 |           |                |           |
     A1      -->|           |--------------->| A1        |
           PD4 |           |                |           |
     A2      -->|           |--------------->| A2        |
               |           |                |           |
               +-----------+                +-----------+
                                                 | | |
                                                 v v v
                                           To MPU6050 Sensors
```

## TCA9548A to MPU6050 Sensors Connection

```
             TCA9548A Multiplexer              MPU6050 Sensors
               +-----------+                +-----------+
          SD0 |           |                |           |
     Sensor0 -->|           |<------------->| SDA (S0)  |
          SC0 |           |                |           |
            -->|           |<------------->| SCL (S0)  |
               |           |                +-----------+
          SD1 |           |                +-----------+
     Sensor1 -->|           |<------------->| SDA (S1)  |
          SC1 |           |                |           |
            -->|           |<------------->| SCL (S1)  |
               |           |                +-----------+
          SD2 |           |                +-----------+
     Sensor2 -->|           |<------------->| SDA (S2)  |
          SC2 |           |                |           |
            -->|           |<------------->| SCL (S2)  |
               |           |                +-----------+
          SD3 |           |                +-----------+
     Sensor3 -->|           |<------------->| SDA (S3)  |
          SC3 |           |                |           |
            -->|           |<------------->| SCL (S3)  |
               |           |                +-----------+
          SD4 |           |                +-----------+
     Sensor4 -->|           |<------------->| SDA (S4)  |
          SC4 |           |                |           |
            -->|           |<------------->| SCL (S4)  |
               |           |                +-----------+
               +-----------+
```

## ATmega32a to ESP32 Connection

```
                 ATmega32a                       ESP32
               +-----------+                +-----------+
           PD0 |           |                |           |
     UART RX -->|           |<-------------+| GPIO17 TX |
           PD1 |           |                |           |
     UART TX -->|           |+------------->| GPIO16 RX |
               |           |                |           |
           PB1 |           |                |           |
     Control -->|           |<------------->| GPIO26    |
               |           |                |           |
           PC5 |           |                |           |
  Handshake  -->|           |<------------->| GPIO27    |
               |           |                |           |
           PD6 |           |                |           |
  ESP Reset  -->|           |--------------->| EN        |
               |           |                |           |
               +-----------+                +-----------+
```

## Power Distribution

```
                   Power Supply
                   +-------------------+
                   |                   |
                   |  5V Regulator     |
                   +-------------------+
                           |
                           v
                   +-------------------+
                   |                   |
                   |  3.3V Regulator   |
                   +-------------------+
                     |               |
                     v               v
              +----------+    +---------------+
              |          |    |               |
              | ATmega32a|    | 3.3V Devices  |
              | LCD      |    | (MPU6050 x5,  |
              |          |    |  TCA9548A,    |
              |          |    |  ESP32)       |
              +----------+    +---------------+
```

## Button Connections

```
           5V
            |
            v
           10kΩ
            |
            +----------------+
            |                |
            v                v
     +------------+    +------------+
     | Calibration|    | Mode Select|
     | Button (PA6)|    | Button (PA7)|
     +------------+    +------------+
            |                |
            v                v
           GND              GND
```

## LED Indicators

```
           5V
            |
            v
      +-----+-----+-----+-----+
      |     |     |     |     |
     330Ω  330Ω  330Ω  330Ω  330Ω
      |     |     |     |     |
      v     v     v     v     v
    +---+ +---+ +---+ +---+ +---+
    |PWR| |GRN| |RED| |BLU| |   |
    |LED| |LED| |LED| |LED| |   |
    +---+ +---+ +---+ +---+ +---+
      |     |     |     |     |
      |    PC2   PC3   PC4   PD5
     GND    |     |     |     |
            v     v     v     v
           GND   GND   GND   GND
```
