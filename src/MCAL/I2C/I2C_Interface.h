#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "STD_TYPES.h"

// Basic I2C functions
void I2C_voidInit(void);
u8 I2C_u8Start(void);
void I2C_voidStop(void);
u8 I2C_u8GetStatus(void);

// Low-level byte operations
u8 TWI_WriteByteRaw(u8 u8Data);
u8 TWI_ReadByteRaw(u8 ack, u8 *out);

// High-level device communication functions
u8 I2C_u8WriteReg(u8 addr7, u8 reg, u8 val);
u8 I2C_u8ReadRegs(u8 addr7, u8 reg, u8* buf, u8 len);
u8 I2C_u8WriteByte(u8 addr7, u8 val);
u8 I2C_u8ReadByte(u8 addr7, u8* val);

// Direct address communication (for simple devices)
u8 I2C_u8WriteByteAddr(u8 addr7, u8 val);
u8 I2C_u8ReadByteAddr(u8 addr7, u8 *val);

// Compatibility functions (legacy support)
void I2C_voidStart(void);
void I2C_voidWrite(u8 u8Data);
u8 I2C_u8ReadWithACK(void);
u8 I2C_u8ReadWithNACK(void);

// Helper initialization function
void I2C_helpers_init(u32 i2c_hz);

#endif // I2C_INTERFACE_H