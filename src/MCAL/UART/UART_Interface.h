#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H



// UART Functions
void UART_voidInit(void);
void UART_voidSendByte(u8 u8Data);
void UART_voidSendChar(char c);
void UART_voidSendString(const char* str);  // Added const qualifier
void UART_voidSendNumber(s16 number);

#endif
