# Smart Glove System

A real-time gesture recognition system using ATmega32 microcontroller and multiple MPU6050 sensors.

## Hardware Requirements

- **Microcontroller**: ATmega32 @ 8MHz
- **Sensors**: 5x MPU6050 (accelerometer/gyroscope)
- **I2C Multiplexer**: TCA9548A (8-channel)
- **Display**: 16x2 LCD (LMB161A)
- **Communication**: UART to ESP32
- **Programmer**: USBasp or compatible

## Software Requirements

- **avr-gcc toolchain**
- **avrdude** (for programming)
- **make** (for building)

### Optional Tools

- **cppcheck** (for static analysis)
- **astyle** (for code formatting)

## Building the Project

### Quick Start

```bash
# Build the project
make

# Program the microcontroller
make program

# Clean build files
make clean
```

### Available Commands

| Command | Description |
|---------|-------------|
| `make` or `make all` | Build the complete project |
| `make clean` | Remove all build files |
| `make rebuild` | Clean and build |
| `make program` | Program the microcontroller |
| `make program-verify` | Program with verification |
| `make size` | Show memory usage information |
| `make check` | Syntax check without building |
| `make help` | Show all available commands |

### Advanced Commands

| Command | Description |
|---------|-------------|
| `make read-fuses` | Read current fuse settings |
| `make set-fuses` | Set fuses for 8MHz internal oscillator |
| `make backup` | Create timestamped source backup |
| `make analyze` | Run static analysis (requires cppcheck) |
| `make format` | Format source code (requires astyle) |
| `make debug` | Show Makefile configuration |

## Configuration

### Microcontroller Settings

- **Target**: ATmega32
- **Clock**: 8MHz internal oscillator
- **Programmer**: USBasp (default)

### Fuse Settings

For 8MHz internal oscillator:
- **Low Fuse**: 0xE4
- **High Fuse**: 0xD9

Set fuses with: `make set-fuses`

### Customization

Edit the Makefile to change:

```makefile
# Change target microcontroller
MCU = atmega32

# Change clock frequency
F_CPU = 8000000UL

# Change programmer type
PROGRAMMER = usbasp

# Change programmer port
PORT = /dev/ttyUSB0
```

## Project Structure

```
├── Makefile                    # Build configuration
├── README.md                   # This file
├── main_gesture_stream.c       # Main application
├── gesture_stream.*            # Core gesture functionality
├── gesture_stream_platform.*   # Platform-specific code
├── MPU6050_*                   # MPU6050 sensor driver
├── TCA9548A_*                  # I2C multiplexer driver
├── LCD_*                       # LCD display driver
├── UART_*                      # UART communication
├── I2C_*                       # I2C protocol implementation
├── DIO_*                       # Digital I/O operations
├── math_utils.*                # Mathematical utilities
└── build/                      # Build output directory
    ├── *.o                     # Object files
    ├── *.d                     # Dependency files
    ├── LCD_MPU.elf            # Executable
    ├── LCD_MPU.hex            # Intel HEX file
    ├── LCD_MPU.lss            # Extended listing
    └── LCD_MPU.map            # Memory map
```

## Communication Protocol

The system sends binary packets to ESP32:

```c
// Packet Structure (27 bytes)
[Header: 0xAA55][Version: 0x01][Timestamp: 4 bytes]
[10 x Angle Values: 20 bytes][CRC16: 2 bytes]
```

- **Update Rate**: 25Hz (40ms intervals)
- **Angles**: Roll/pitch for 5 fingers in centidegrees
- **Checksum**: CRC16-CCITT for data integrity

## Troubleshooting

### Build Issues

1. **"avr-gcc not found"**
   ```bash
   # Install AVR toolchain
   sudo apt-get install gcc-avr avr-libc avrdude
   ```

2. **"Permission denied" during programming**
   ```bash
   # Add user to dialout group
   sudo usermod -a -G dialout $USER
   # Then logout and login again
   ```

3. **"Device not found"**
   ```bash
   # Check programmer connection
   make read-fuses
   ```

### Runtime Issues

1. **"Init failed" on LCD**
   - Check I2C connections
   - Verify sensor power supply
   - Check TCA9548A wiring

2. **"No reply" from ESP32**
   - Verify UART connections
   - Check ESP32 firmware
   - Test with oscilloscope/logic analyzer

3. **Erratic sensor readings**
   - Run sensor calibration
   - Check for loose connections
   - Verify power supply stability

## Development

### Adding New Features

1. Add source files to `SOURCES` in Makefile
2. Add header files to `HEADERS` in Makefile
3. Run `make rebuild` to test

### Code Quality

```bash
# Check syntax
make check

# Run static analysis
make analyze

# Format code
make format

# Create backup before changes
make backup
```

## License

This project is developed for educational and research purposes.
