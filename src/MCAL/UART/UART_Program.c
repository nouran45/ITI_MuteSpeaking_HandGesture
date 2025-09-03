#include "UART_Interface.h"
#include "../DIO/DIO_Register.h"
#include <avr/io.h>
#include <util/delay.h>

// UART initialization for 9600 baud rate with 8MHz CPU
void UART_voidInit(void) {
    // Set baud rate to 9600 (UBRR = F_CPU/(16*BAUD) - 1)
    // For 8MHz and 9600 baud: UBRR = 8000000/(16*9600) - 1 = 51
    UBRRH = 0;
    UBRRL = 51;
    
    // Enable transmitter
    UCSRB = (1 << TXEN);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

// Send single byte
void UART_voidSendByte(u8 u8Data) {
    // Wait for empty transmit buffer
    while (!(UCSRA & (1 << UDRE)));
    
    // Put data into buffer, sends the data
    UDR = u8Data;
}

// Send string
void UART_voidSendString(char* str) {
    while (*str) {
        UART_voidSendByte(*str);
        str++;
    }
}

// Send number
void UART_voidSendNumber(s16 number) {
    char buffer[10];
    char* ptr = buffer;
    
    // Handle negative numbers
    if (number < 0) {
        UART_voidSendByte('-');
        number = -number;
    }
    
    // Convert number to string (reverse order)
    if (number == 0) {
        UART_voidSendByte('0');
        return;
    }
    
    while (number > 0) {
        *ptr++ = (number % 10) + '0';
        number /= 10;
    }
    
    // Send digits in correct order
    while (ptr > buffer) {
        ptr--;
        UART_voidSendByte(*ptr);
    }
}
