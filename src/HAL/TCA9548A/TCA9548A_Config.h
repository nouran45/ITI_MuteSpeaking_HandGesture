#ifndef TCA9548A_CONFIG_H
#define TCA9548A_CONFIG_H

#include "STD_TYPES.h"

// TCA9548A I2C Address (7-bit)
// Hardware address pins A2=0, A1=0, A0=0 -> Address = 0x70
#define TCA9548A_I2C_ADDRESS    0x70

// Number of channels available on TCA9548A
#define TCA9548A_MAX_CHANNELS   8

// Channel mapping for sensors
#define TCA9548A_THUMB_CHANNEL      0    // Channel 0 - Thumb sensor
#define TCA9548A_INDEX_CHANNEL      1    // Channel 1 - Index finger sensor
#define TCA9548A_MIDDLE_CHANNEL     2    // Channel 2 - Middle finger sensor
#define TCA9548A_RING_CHANNEL       3    // Channel 3 - Ring finger sensor
#define TCA9548A_PALM_CHANNEL       4    // Channel 4 - Palm/hand orientation sensor

// Control register values
#define TCA9548A_CHANNEL_0      0x01    // 0000 0001
#define TCA9548A_CHANNEL_1      0x02    // 0000 0010
#define TCA9548A_CHANNEL_2      0x04    // 0000 0100
#define TCA9548A_CHANNEL_3      0x08    // 0000 1000
#define TCA9548A_CHANNEL_4      0x10    // 0001 0000
#define TCA9548A_CHANNEL_5      0x20    // 0010 0000
#define TCA9548A_CHANNEL_6      0x40    // 0100 0000
#define TCA9548A_CHANNEL_7      0x80    // 1000 0000
#define TCA9548A_NO_CHANNEL     0x00    // Disable all channels

// Reset configuration
#define TCA9548A_RESET_ACTIVE_LOW   1   // Reset pin is active low

// Timeout for I2C operations (in milliseconds)
#define TCA9548A_TIMEOUT_MS     100

#endif // TCA9548A_CONFIG_H

