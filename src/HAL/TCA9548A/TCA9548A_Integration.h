#ifndef TCA9548A_INTEGRATION_H
#define TCA9548A_INTEGRATION_H

#include "STD_TYPES.h"
#include "TCA9548A_Interface.h"

/**
 * @brief Callback function for MPU6050 driver to select multiplexer channel
 * @param u8Channel: Channel number to select (0-7)
 * @note This function is designed to be used as mpu_mux_select_fn callback
 */
void TCA9548A_SelectChannelCallback(u8 u8Channel);

/**
 * @brief Initialize TCA9548A and register sensors with MPU6050 driver
 * @return TCA9548A_Status_t - Status of initialization
 * @note This function sets up the complete multi-sensor system
 */
TCA9548A_Status_t TCA9548A_InitializeSmartGloveSystem(void);

/**
 * @brief Test all sensors on all channels
 * @return u8 - Number of sensors found (0-5)
 * @note Useful for system diagnostics
 */
u8 TCA9548A_TestAllSensors(void);

#endif // TCA9548A_INTEGRATION_H
