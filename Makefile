# ===== Project =====
TARGET := firmware
MCU    := atmega32
F_CPU  := 8000000UL

# ===== Toolchain (WinAVR on PATH) =====
CC      := avr-gcc
OBJCOPY := avr-objcopy
SIZE    := avr-size
AVRDUDE := avrdude
AVRDUDE_CONF := C:\iti\IMT_SDK_Win_64\WinAVR\bin\avrdude.conf

# ===== Sources (one and two levels under src/) =====
SRC := \
  $(wildcard src/*.c) \
  $(wildcard src/*/*.c) \
  $(wildcard src/*/*/*.c)

# ===== Include dirs (LIST ONLY DIRECTORIES) =====
INC_DIRS := \
  . \
  include \
  src \
  src/APP \
  src/HAL \
  src/MCAL \
  src/LIB \
  src/HAL/LCD \
  src/HAL/MPU6050 \
  src/HAL/TCA9548A \
  src/MCAL/DIO \
  src/MCAL/I2C \
  src/MCAL/UART
INC := $(addprefix -I,$(INC_DIRS))

# ===== Flags =====
CFLAGS  := -std=gnu99 -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(INC) -Wall -Wextra -ffunction-sections -fdata-sections
LDFLAGS := -mmcu=$(MCU) -Wl,--gc-sections
LDLIBS  := -lm

# ===== Programmer (edit as needed) =====
PROGRAMMER := usbasp
PORT       :=
BAUD       :=

AVRDUDE_FLAGS := -C "$(AVRDUDE_CONF)" -p m32 -c $(PROGRAMMER)
ifneq ($(PORT),)
  AVRDUDE_FLAGS += -P $(PORT)
endif
ifneq ($(BAUD),)
  AVRDUDE_FLAGS += -b $(BAUD)
endif

# ===== Outputs =====
OBJ := $(SRC:.c=.o)
ELF := $(TARGET).elf
HEX := $(TARGET).hex

# ===== Rules =====
all: $(HEX)
	@$(SIZE) -C --mcu=$(MCU) $(ELF)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

flash: $(HEX)
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(HEX):i

clean:
	-rm -f $(OBJ) $(ELF) $(HEX)

.PHONY: all flash clean
