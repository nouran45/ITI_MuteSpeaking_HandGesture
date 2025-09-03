MCU=atmega32a
F_CPU=8000000UL
CC=avr-gcc
CFLAGS=-Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -I src/LIB -I src/MCAL -I src/HAL
OBJ2HEX=avr-objcopy

# Directory structure
SRC_DIR=src
LIB_DIR=$(SRC_DIR)/LIB
MCAL_DIR=$(SRC_DIR)/MCAL
HAL_DIR=$(SRC_DIR)/HAL
APP_DIR=$(SRC_DIR)/APP
BUILD_DIR=build

# Target executable
TARGET=main

# Source files
SRC_FILES=src/MCAL/DIO/DIO_PROGRAM.c \
          src/MCAL/I2C/I2C_Program.c \
          src/HAL/LCD/LCD_Program.c \
          src/APP/main.c \
          src/HAL/MPU6050/MPU6050_Program.c \
          src/MCAL/UART/UART_Program.c

# Object files
OBJ=$(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRC_FILES)))

all: directories $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex

# Create necessary directories
directories:
	mkdir -p $(BUILD_DIR)

# Pattern rule to compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Special rules for files in subdirectories
$(BUILD_DIR)/DIO_PROGRAM.o: $(MCAL_DIR)/DIO/DIO_PROGRAM.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/I2C_Program.o: $(MCAL_DIR)/I2C/I2C_Program.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/UART_Program.o: $(MCAL_DIR)/UART/UART_Program.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/LCD_Program.o: $(HAL_DIR)/LCD/LCD_Program.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/MPU6050_Program.o: $(HAL_DIR)/MPU6050/MPU6050_Program.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(APP_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create ELF file
$(BUILD_DIR)/$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Convert ELF to HEX
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJ2HEX) -O ihex -R .eeprom $< $@

# Flash the HEX file to the microcontroller
flash: $(BUILD_DIR)/$(TARGET).hex
	avrdude -c usbasp -p $(MCU) -U flash:w:$<

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
