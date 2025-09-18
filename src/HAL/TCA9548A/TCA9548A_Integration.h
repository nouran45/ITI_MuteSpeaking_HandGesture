#ifndef TCA9548A_INTEGRATION_H
#define TCA9548A_INTEGRATION_H

#include "STD_TYPES.h"
#include "MPU6050_INTERFACE.h"
#include "TCA9548A_Interface.h"

void TCA9548A_SelectChannelCallback(u8 u8Channel);
TCA9548A_Status_t TCA9548A_InitializeSmartGloveSystem(void);
u8 TCA9548A_TestAllSensors(void);
void MPU6050_RegisterAllSensors(void);

#endif // TCA9548A_INTEGRATION_H
