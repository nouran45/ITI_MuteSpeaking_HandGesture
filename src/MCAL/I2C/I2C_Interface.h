#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "STD_TYPES.h"

void I2C_voidInit(void);
void I2C_voidStart(void);
void I2C_voidStop(void);
void I2C_voidWrite(u8 u8Data);
u8 I2C_u8ReadWithACK(void);
u8 I2C_u8ReadWithNACK(void);
u8 I2C_u8GetStatus(void);

#endif