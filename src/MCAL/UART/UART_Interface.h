#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "STD_TYPES.h"

// UART Configuration - MEMORY OPTIMIZED
#define UART_BUFFER_SIZE 32    // REDUCED from 64 to save 32 bytes RAM
#define UART_BAUD_RATE 9600

// UART Functions
void UART_voidInit(void);
void UART_voidInitWithInterrupt(void);
void UART_voidSendByte(u8 u8Data);
void UART_voidSendChar(char c);
void UART_voidSendString(const char* str);
void UART_voidSendNumber(s16 number);
void UART_voidSendPacket(u8 *data, u16 length);

// UART Receive Functions
u8 UART_u8ReceiveByte(void);
u8 UART_u8ReceiveByteNonBlocking(u8 *data);
u8 UART_u8DataAvailable(void);
void UART_voidFlushReceiveBuffer(void);

// UART Utility Functions
u8 UART_u8CalculateChecksum(u8 *data, u16 length);

// Interrupt-based receive buffer access
extern volatile u8 UART_RxBuffer[UART_BUFFER_SIZE];
extern volatile u8 UART_RxHead;
extern volatile u8 UART_RxTail;
extern volatile u8 UART_RxFlag;

#endif
