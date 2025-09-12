#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Config.h"
#include "TCA9548A_Integration.h"
#include "I2C_Interface.h"
#include "DIO_Interface.h"
#include "DIO_Register.h"
#include <util/delay.h>

// Internal state tracking
static struct {
    u8 current_channel_mask;
    u8 initialized;
    u8 last_selected_channel;
} tca9548a_state = {0, 0, 0xFF};

// Channel lookup table for single channel selection
static const u8 channel_masks[TCA9548A_MAX_CHANNELS] = {
    TCA9548A_CHANNEL_0,
    TCA9548A_CHANNEL_1,
    TCA9548A_CHANNEL_2,
    TCA9548A_CHANNEL_3,
    TCA9548A_CHANNEL_4,
    TCA9548A_CHANNEL_5,
    TCA9548A_CHANNEL_6,
    TCA9548A_CHANNEL_7
};

/**
 * @brief Internal function to validate channel number
 */
static TCA9548A_Status_t validate_channel(u8 channel) {
    if (channel >= TCA9548A_MAX_CHANNELS) {
        return TCA9548A_ERROR_INVALID_CHANNEL;
    }
    return TCA9548A_OK;
}

/**
 * @brief Internal function to write channel selection to TCA9548A
 */
static TCA9548A_Status_t write_channel_register(u8 channel_mask) {
    // TCA9548A uses a simple write of the channel mask directly to the device
    if (I2C_u8WriteByte(TCA9548A_I2C_ADDRESS, channel_mask)) {
        return TCA9548A_ERROR_I2C;
    }
    
    tca9548a_state.current_channel_mask = channel_mask;
   _delay_ms(TCA9548A_SETTLING_TIME_MS); // ✅ ADDED: Critical delay for mux stability
    return TCA9548A_OK;
}

/**
 * @brief Internal function to read current channel register from TCA9548A
 */
static TCA9548A_Status_t read_channel_register(u8* channel_mask) {
    if (!channel_mask) {
        return TCA9548A_ERROR_I2C;
    }
    
    // Read current channel selection directly from TCA9548A
    // No register address needed - just read one byte
    if (I2C_u8ReadByte(TCA9548A_I2C_ADDRESS, channel_mask)) {
        return TCA9548A_ERROR_I2C;
    }
    
    return TCA9548A_OK;
}

TCA9548A_Status_t TCA9548A_Init(void) {
    // Initialize I2C if not already done
    I2C_helpers_init(100000); // 100 kHz I2C speed
    
    // Configure reset pin as output if used
    #ifdef TCA9548A_RESET_PIN_PORT
    DIO_voidSetPinDirection(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_OUTPUT);
    
    // Perform hardware reset
    if (TCA9548A_RESET_ACTIVE_LOW) {
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_LOW);
        _delay_ms(10);
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_HIGH);
    } else {
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_HIGH);
        _delay_ms(10);
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_LOW);
    }
    _delay_ms(50); // Wait for reset to complete
    #endif
    
    // Test communication with the device
    TCA9548A_Status_t status = TCA9548A_TestCommunication();
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // Initialize by disabling all channels
    status = TCA9548A_DisableAllChannels();
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // Mark as initialized
    tca9548a_state.initialized = 1;
    tca9548a_state.last_selected_channel = 0xFF; // Invalid channel
    
    return TCA9548A_OK;
}

TCA9548A_Status_t TCA9548A_SelectChannel(u8 u8ChannelId) {
    if (!tca9548a_state.initialized) {
        return TCA9548A_ERROR_NOT_INITIALIZED;
    }
    
    TCA9548A_Status_t status = validate_channel(u8ChannelId);
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // Optimization: Don't write if already selected
    if (tca9548a_state.last_selected_channel == u8ChannelId) {
        return TCA9548A_OK;
    }
    
    // Select the channel
    u8 channel_mask = channel_masks[u8ChannelId];
    status = write_channel_register(channel_mask);
    if (status == TCA9548A_OK) {
        tca9548a_state.last_selected_channel = u8ChannelId;
    }
    
    return status;
}

TCA9548A_Status_t TCA9548A_SelectMultipleChannels(u8 u8ChannelMask) {
    if (!tca9548a_state.initialized) {
        return TCA9548A_ERROR_NOT_INITIALIZED;
    }
    
    // Note: u8 parameter can never exceed 0xFF, so no validation needed
    
    TCA9548A_Status_t status = write_channel_register(u8ChannelMask);
    if (status == TCA9548A_OK) {
        tca9548a_state.last_selected_channel = 0xFF; // Multiple channels, invalidate cache
    }
    
    return status;
}

TCA9548A_Status_t TCA9548A_DisableAllChannels(void) {
    if (!tca9548a_state.initialized) {
        return TCA9548A_ERROR_NOT_INITIALIZED;
    }
    
    TCA9548A_Status_t status = write_channel_register(TCA9548A_NO_CHANNEL);
    if (status == TCA9548A_OK) {
        tca9548a_state.last_selected_channel = 0xFF; // No channel selected
    }
    
    return status;
}

u8 TCA9548A_GetActiveChannels(void) {
    if (!tca9548a_state.initialized) {
        return 0xFF; // Error indicator
    }
    
    u8 channel_mask;
    TCA9548A_Status_t status = read_channel_register(&channel_mask);
    if (status != TCA9548A_OK) {
        return 0xFF; // Error indicator
    }
    
    return channel_mask;
}

TCA9548A_ChannelState_t TCA9548A_IsChannelActive(u8 u8ChannelId) {
    if (!tca9548a_state.initialized || validate_channel(u8ChannelId) != TCA9548A_OK) {
        return TCA9548A_CHANNEL_DISABLED;
    }
    
    u8 active_channels = TCA9548A_GetActiveChannels();
    if (active_channels == 0xFF) {
        return TCA9548A_CHANNEL_DISABLED; // Error case
    }
    
    if (active_channels & channel_masks[u8ChannelId]) {
        return TCA9548A_CHANNEL_ENABLED;
    }
    
    return TCA9548A_CHANNEL_DISABLED;
}

TCA9548A_Status_t TCA9548A_HardwareReset(void) {
    #ifdef TCA9548A_RESET_PIN_PORT
    // Perform hardware reset
    if (TCA9548A_RESET_ACTIVE_LOW) {
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_LOW);
        _delay_ms(10);
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_HIGH);
    } else {
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_HIGH);
        _delay_ms(10);
        DIO_voidSetPinValue(TCA9548A_RESET_PIN_PORT, TCA9548A_RESET_PIN, DIO_u8_LOW);
    }
    _delay_ms(50);
    
    // Reinitialize state
    tca9548a_state.current_channel_mask = 0;
    tca9548a_state.last_selected_channel = 0xFF;
    
    return TCA9548A_OK;
    #else
    // Hardware reset not available
    return TCA9548A_SoftwareReset();
    #endif
}

TCA9548A_Status_t TCA9548A_SoftwareReset(void) {
    // Software reset by disabling all channels and reinitializing state
    TCA9548A_Status_t status = write_channel_register(TCA9548A_NO_CHANNEL);
    if (status == TCA9548A_OK) {
        tca9548a_state.current_channel_mask = 0;
        tca9548a_state.last_selected_channel = 0xFF;
    }
    
    return status;
}

TCA9548A_Status_t TCA9548A_TestCommunication(void) {
    u8 current_channels;
    
    // TCA9548A uses direct byte communication, not register-based
    // Try to read current channel selection directly
    if (I2C_u8ReadByte(TCA9548A_I2C_ADDRESS, &current_channels)) {
        return TCA9548A_ERROR_I2C;
    }
    
    // Test write capability by selecting no channels and reading back
    if (I2C_u8WriteByte(TCA9548A_I2C_ADDRESS, TCA9548A_NO_CHANNEL)) {
        return TCA9548A_ERROR_I2C;
    }
    
    // Read back to verify the write worked
    u8 readback;
    if (I2C_u8ReadByte(TCA9548A_I2C_ADDRESS, &readback)) {
        return TCA9548A_ERROR_I2C;
    }
    
    // Verify that no channels are selected (should be 0x00)
    if (readback != TCA9548A_NO_CHANNEL) {
        return TCA9548A_ERROR_I2C;
    }
    
    // Restore original channel selection
    if (I2C_u8WriteByte(TCA9548A_I2C_ADDRESS, current_channels)) {
        return TCA9548A_ERROR_I2C;
    }
    
    return TCA9548A_OK;
}

TCA9548A_Status_t TCA9548A_ReadRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8* pu8Data) {
    if (!tca9548a_state.initialized) {
        return TCA9548A_ERROR_NOT_INITIALIZED;
    }
    
    if (!pu8Data) {
        return TCA9548A_ERROR_I2C;
    }
    
    TCA9548A_Status_t status = validate_channel(u8Channel);
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // NOTE: Channel selection is handled by MPU6050 callback system
    // Do NOT select channel here to avoid conflicts
    
    // Read from the device (callback will handle channel selection)
    if (I2C_u8ReadRegs(u8DeviceAddr, u8RegAddr, pu8Data, 1)) {
        return TCA9548A_ERROR_I2C;
    }
    
    return TCA9548A_OK;
}

TCA9548A_Status_t TCA9548A_WriteRegister(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8 u8Data) {
    if (!tca9548a_state.initialized) {
        return TCA9548A_ERROR_NOT_INITIALIZED;
    }
    
    TCA9548A_Status_t status = validate_channel(u8Channel);
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // NOTE: Channel selection is handled by MPU6050 callback system
    // Do NOT select channel here to avoid conflicts
    
    // Write to the device (callback will handle channel selection)
    if (I2C_u8WriteReg(u8DeviceAddr, u8RegAddr, u8Data)) {
        return TCA9548A_ERROR_I2C;
    }
    
    return TCA9548A_OK;
}

TCA9548A_Status_t TCA9548A_ReadMultipleRegisters(u8 u8Channel, u8 u8DeviceAddr, u8 u8RegAddr, u8* pu8Data, u8 u8Length) {
    if (!tca9548a_state.initialized) {
        return TCA9548A_ERROR_NOT_INITIALIZED;
    }
    
    if (!pu8Data || u8Length == 0) {
        return TCA9548A_ERROR_I2C;
    }
    
    TCA9548A_Status_t status = validate_channel(u8Channel);
    if (status != TCA9548A_OK) {
        return status;
    }
    
    // NOTE: Channel selection is handled by MPU6050 callback system
    // Do NOT select channel here to avoid conflicts
    
    // Read from the device (callback will handle channel selection)
    if (I2C_u8ReadRegs(u8DeviceAddr, u8RegAddr, pu8Data, u8Length)) {
        return TCA9548A_ERROR_I2C;
    }
    
    return TCA9548A_OK;
}

const char* TCA9548A_GetStatusString(TCA9548A_Status_t status) {
    switch (status) {
        case TCA9548A_OK:
            return "OK";
        case TCA9548A_ERROR_I2C:
            return "I2C Error";
        case TCA9548A_ERROR_INVALID_CHANNEL:
            return "Invalid Channel";
        case TCA9548A_ERROR_TIMEOUT:
            return "Timeout";
        case TCA9548A_ERROR_NOT_INITIALIZED:
            return "Not Initialized";
        default:
            return "Unknown Error";
    }
}
