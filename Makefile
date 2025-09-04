MCU=atmega32a
F_CPU=8000000UL
CC=avr-gcc
CFLAGS=-Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -I src/LIB -I src/MCAL -I src/HAL
OBJ2HEX=avr-objcopy

SRC_DIR=src
BUILD_DIR=build
TARGET=main

# === List every .c you want linked into THE APP (no tests here) ===
SRC_FILES= \
  src/MCAL/DIO/DIO_PROGRAM.c \
  src/MCAL/I2C/I2C_Program.c \
  src/MCAL/UART/UART_Program.c \
  src/HAL/LCD/LCD_Program.c \
  src/HAL/MPU6050/MPU6050_Program.c \
  src/APP/main.c

# If you use the I2C helpers (I2C_u8ReadRegs / I2C_u8WriteReg),
# add their implementation here:
#   src/MCAL/I2C/i2c_helpers.c
# (uncomment if you created it)
# SRC_FILES += src/MCAL/I2C/i2c_helpers.c

# Objects mirror the source tree under build/
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Make sure the dir structure exists
DIRS := $(sort $(dir $(OBJ)))

.DEFAULT_GOAL := all

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJ2HEX) -O ihex -R .eeprom $< $@

# Compile rule (auto-creates subdirs in build/)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(DIRS):
	mkdir -p $@

flash: $(BUILD_DIR)/$(TARGET).hex
	avrdude -c usbasp -p $(MCU) -U flash:w:$<

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all flash clean
