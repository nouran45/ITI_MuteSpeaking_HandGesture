# ITI_MuteSpeaking_HandGesture

End-to-end gesture-to-speech stack using AVR (5×MPU6050 over I²C mux) + ESP32 (orientation fusion, classifier, output).

# Smart Glove Project

## Project Structure

The project is organized with the following directory structure:

- `src/` - Source code
  - `MCAL/` - Microcontroller Abstraction Layer (low-level drivers)
  - `HAL/` - Hardware Abstraction Layer (device drivers)
  - `APP/` - Application layer code
  - `LIB/` - Common libraries and utilities
- `include/` - Header files
- `docs/` - Documentation files
- `tools/` - Build and utility tools
- `tests/` - Test code
- `scripts/` - Python scripts for simulation and data visualization

## File Organization Guidelines

### MCAL Layer
- DIO (Digital I/O)
- I2C
- UART
- Any other microcontroller peripherals

### HAL Layer
- MPU6050
- LCD
- TCA9548A (I2C Multiplexer)
- Other hardware devices

### APP Layer
- Main application code
- Feature extraction
- Gesture recognition
- User interface

### LIB Layer
- Standard types
- Bit manipulation
- Math utilities
- Other common utilities

## Build Instructions

[Add build instructions here]

## Usage

[Add usage instructions here]
