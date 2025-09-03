#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "STD_TYPES.h"

// UART Functions
void UART_voidInit(void);
void UART_voidSendByte(u8 u8Data);
void UART_voidSendString(char* str);
void UART_voidSendNumber(s16 number);

#endif
