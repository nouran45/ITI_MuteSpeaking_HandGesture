#include "STD_TYPES.h"
#include "MPU6050_INTERFACE.h"
#include "I2C_Interface.h"
#include "TCA9548A_Integration.h"
#include "TCA9548A_Interface.h"
#include "TCA9548A_Config.h"
#include "UART_Interface.h"
#include <util/delay.h>

#define MPU6050_I2C_ADDRESS 0x68
#define WHO_AM_I_REG        0x75

// Forward declaration for the sensor registration function
void MPU6050_RegisterAllSensors(void);

void TCA9548A_SelectChannelCallback(u8 u8Channel){
    if (u8Channel == 0xFF){
        (void)TCA9548A_DisableAllChannels();
    }else if (u8Channel < 8){
        (void)TCA9548A_SelectChannel(u8Channel);
    }
    _delay_ms(TCA9548A_SETTLING_TIME_MS);
}

TCA9548A_Status_t TCA9548A_InitializeSmartGloveSystem(void){
    TCA9548A_Status_t st = TCA9548A_Init();
    if (st != TCA9548A_OK) return st;

    // Register callback with MPU driver for channel switching
    MPU6050_voidSetMuxSelector(TCA9548A_SelectChannelCallback);
    
    // Register all sensors with the MPU6050 driver
    MPU6050_RegisterAllSensors();

    // Basic per-channel bring-up: ensure WHO_AM_I responds on 0..4
    for (u8 ch=0; ch<5; ch++){
        u8 who=0;
        if (TCA9548A_ReadRegister(ch, MPU6050_I2C_ADDRESS, WHO_AM_I_REG, &who) != TCA9548A_OK || who!=0x68){
            UART_voidSendString("MPU missing on CH"); UART_voidSendNumber(ch); UART_voidSendString("\r\n");
        }
    }
    return TCA9548A_OK;
}

u8 TCA9548A_TestAllSensors(void){
    u8 ok=0;
    for (u8 ch=0; ch<5; ch++){
        u8 who=0;
        if (TCA9548A_ReadRegister(ch, MPU6050_I2C_ADDRESS, WHO_AM_I_REG, &who) == TCA9548A_OK && who==0x68){
            ok++;
        }
    }
    (void)TCA9548A_DisableAllChannels();
    return ok;
}
