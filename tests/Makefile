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
TESTS_DIR=tests

# Define test targets
TEST_TARGETS= lcd_test_only LCD_diagnostic LCD_diagnostic_tool

# Default target is all tests
all: $(TEST_TARGETS)

# Create necessary directories
directories:
	mkdir -p $(BUILD_DIR)

# LCD test only
lcd_test_only: directories
	$(CC) $(CFLAGS) -c $(TESTS_DIR)/lcd_test_only.c -o $(BUILD_DIR)/lcd_test_only.o
	$(CC) $(CFLAGS) -c $(MCAL_DIR)/DIO/DIO_PROGRAM.c -o $(BUILD_DIR)/DIO_PROGRAM.o
	$(CC) $(CFLAGS) -c $(HAL_DIR)/LCD/LCD_Program.c -o $(BUILD_DIR)/LCD_Program.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/lcd_test_only.o $(BUILD_DIR)/DIO_PROGRAM.o $(BUILD_DIR)/LCD_Program.o -o $(BUILD_DIR)/lcd_test_only.elf
	$(OBJ2HEX) -O ihex -R .eeprom $(BUILD_DIR)/lcd_test_only.elf $(BUILD_DIR)/lcd_test_only.hex

# LCD diagnostic
LCD_diagnostic: directories
	$(CC) $(CFLAGS) -c $(TESTS_DIR)/LCD_diagnostic.c -o $(BUILD_DIR)/LCD_diagnostic.o
	$(CC) $(CFLAGS) -c $(MCAL_DIR)/DIO/DIO_PROGRAM.c -o $(BUILD_DIR)/DIO_PROGRAM.o
	$(CC) $(CFLAGS) -c $(HAL_DIR)/LCD/LCD_Program.c -o $(BUILD_DIR)/LCD_Program.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/LCD_diagnostic.o $(BUILD_DIR)/DIO_PROGRAM.o $(BUILD_DIR)/LCD_Program.o -o $(BUILD_DIR)/LCD_diagnostic.elf
	$(OBJ2HEX) -O ihex -R .eeprom $(BUILD_DIR)/LCD_diagnostic.elf $(BUILD_DIR)/LCD_diagnostic.hex

# LCD diagnostic tool
LCD_diagnostic_tool: directories
	$(CC) $(CFLAGS) -c $(TESTS_DIR)/LCD_diagnostic_tool.c -o $(BUILD_DIR)/LCD_diagnostic_tool.o
	$(CC) $(CFLAGS) -c $(MCAL_DIR)/DIO/DIO_PROGRAM.c -o $(BUILD_DIR)/DIO_PROGRAM.o
	$(CC) $(CFLAGS) -c $(HAL_DIR)/LCD/LCD_Program.c -o $(BUILD_DIR)/LCD_Program.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/LCD_diagnostic_tool.o $(BUILD_DIR)/DIO_PROGRAM.o $(BUILD_DIR)/LCD_Program.o -o $(BUILD_DIR)/LCD_diagnostic_tool.elf
	$(OBJ2HEX) -O ihex -R .eeprom $(BUILD_DIR)/LCD_diagnostic_tool.elf $(BUILD_DIR)/LCD_diagnostic_tool.hex

# Flash specific test (e.g., make flash_lcd_test_only)
flash_%: %
	avrdude -c usbasp -p $(MCU) -U flash:w:$(BUILD_DIR)/$<.hex

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all directories clean $(TEST_TARGETS)
