#ifndef TCA9548A_INTERFACE_H 
#define TCA9548A_INTERFACE_H

#include "STD_TYPES.h"
#include "TCA9548A_Config.h"

// Status enumeration for TCA9548A operations
typedef enum {
    TCA9548A_OK = 0,
    TCA9548A_ERROR_I2C = 1,
    TCA9548A_ERROR_INVALID_CHANNEL = 2,
    TCA9548A_ERROR_TIMEOUT = 3,
    TCA9548A_ERROR_NOT_INITIALIZED = 4
} TCA9548A_Status_t;

// Channel state enumeration
typedef enum {
    TCA9548A_CHANNEL_DISABLED = 0,
    TCA9548A_CHANNEL_ENABLED = 1
} TCA9548A_ChannelState_t;

/**
 * @brief Initialize the TCA9548A I2C multiplexer
 * @param None
 * @return TCA9548A_Status_t - Status of initialization
 * @note This function must be called before using any other TCA9548A functions
 */
TCA9548A_Status_t TCA9548A_Init(void);

/**
 * @brief Select a specific channel on the multiplexer
 * @param u8ChannelId: Channel number (0-7)
 * @return TCA9548A_Status_t - Status of channel selection
 * @note Only one channel can be active at a time
 */
TCA9548A_Status_t TCA9548A_SelectChannel(u8 u8ChannelId);

/**
 * @brief Enable multiple channels simultaneously
 * @param u8ChannelMask: Bitmask of channels to enable (bit 0 = channel 0, etc.)
 * @return TCA9548A_Status_t - Status of channel selection
 * @note Use with caution - multiple channels with same I2C address will conflict
 */
TCA9548A_Status_t TCA9548A_SelectMultipleChannels(u8 u8ChannelMask);

/**
 * @brief Disable all channels on the multiplexer
 * @param None
 * @return TCA9548A_Status_t - Status of operation
 * @note Useful for power saving or between sensor operations
 */
TCA9548A_Status_t TCA9548A_DisableAllChannels(void);

/**
 * @brief Get the currently active channel(s)
 * @param None
 * @return u8 - Bitmask of active channels (0 = no channels active)
 * @note Returns 0xFF on error
 */
u8 TCA9548A_GetActiveChannels(void);

/**
 * @brief Check if a specific channel is currently active
 * @param u8ChannelId: Channel number (0-7)
 * @return TCA9548A_ChannelState_t - Channel state (enabled/disabled)
 */
TCA9548A_ChannelState_t TCA9548A_IsChannelActive(u8 u8ChannelId);

/**
 * @brief Reset the TCA9548A multiplexer (hardware reset)
 * @param None
 * @return TCA9548A_Status_t - Status of reset operation
 * @note Requires reset pin to be connected and configured
 */
TCA9548A_Status_t TCA9548A_HardwareReset(void);

/**
 * @brief Software reset - disable all channels and reinitialize
 * @param None
 * @return TCA9548A_Status_t - Status of reset operation
 */
TCA9548A_Status_t TCA9548A_SoftwareReset(void);

/**
 * @brief Test if the TCA9548A is responding on the I2C bus
 * @param None
 * @return TCA9548A_Status_t - Status of communication test
 */
TCA9548A_Status_t TCA9548A_TestCommunication(void);

/**
 * @brief Read from a specific sensor on a specific channel
 * @param u8Channel: Channel number (0-7)
 * @param u8DeviceAddr: 7-bit I2C address of the device
 * @param u8RegAddr: Register address to read from
 * @param pu8Data: Pointer to store the read data
 * @return TCA9548A_Status_t - Status of read operation
 */
TCA9548A_Status_t TCA9548A_ReadRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8* pu8Data);

/**
 * @brief Write to a specific sensor on a specific channel
 * @param u8Channel: Channel number (0-7)
 * @param u8DeviceAddr: 7-bit I2C address of the device
 * @param u8RegAddr: Register address to write to
 * @param u8Data: Data to write
 * @return TCA9548A_Status_t - Status of write operation
 */
TCA9548A_Status_t TCA9548A_WriteRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8 u8Data);

/**
 * @brief Read multiple registers from a specific sensor on a specific channel
 * @param u8Channel: Channel number (0-7)
 * @param u8DeviceAddr: 7-bit I2C address of the device
 * @param u8RegAddr: Starting register address
 * @param pu8Data: Pointer to buffer for read data
 * @param u8Length: Number of bytes to read
 * @return TCA9548A_Status_t - Status of read operation
 */
TCA9548A_Status_t TCA9548A_ReadMultipleRegisters(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8* pu8Data, u8 u8Length);

/**
 * @brief Get string representation of status code (for debugging)
 * @param status: Status code to convert
 * @return const char* - String representation
 */
const char* TCA9548A_GetStatusString(TCA9548A_Status_t status);

#endif // TCA9548A_INTERFACE_H