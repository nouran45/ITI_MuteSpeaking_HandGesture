#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "UART_Interface.h"
#include "UART_Register.h"
#include "DIO_Interface.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Global variables for interrupt-based UART receive
volatile u8 UART_RxBuffer[UART_BUFFER_SIZE];
volatile u8 UART_RxHead = 0;
volatile u8 UART_RxTail = 0;
volatile u8 UART_RxFlag = 0;

// UART Receive Complete Interrupt Service Routine
ISR(USART_RXC_vect) {
    u8 received_byte = UDR;  // Read the received byte
    
    // Calculate next head position
    u8 next_head = (UART_RxHead + 1) % UART_BUFFER_SIZE;
    
    // Store byte if buffer is not full
    if (next_head != UART_RxTail) {
        UART_RxBuffer[UART_RxHead] = received_byte;
        UART_RxHead = next_head;
        UART_RxFlag = 1;  // Set flag to indicate data available
    }
    // If buffer is full, byte is discarded (buffer overflow)
}

// UART initialization for 9600 baud rate with 8MHz CPU
void UART_voidInit(void) {
    // Set pin directions: PD0 (RXD) as input, PD1 (TXD) as output
    DIO_voidSetPinDirection(DIO_u8_PORTD, DIO_u8_PIN0, DIO_u8_INPUT);
    DIO_voidSetPinDirection(DIO_u8_PORTD, DIO_u8_PIN1, DIO_u8_OUTPUT);
    
    // Set baud rate to 9600 (UBRR = F_CPU/(16*BAUD) - 1)
    // For 8MHz and 9600 baud: UBRR = 8000000/(16*9600) - 1 = 51
    UBRRH = 0;
    UBRRL = 51;
    
    // Enable transmitter and receiver
    UCSRB = (1 << RXEN) | (1 << TXEN);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
    
    // Clear receive buffer
    UART_voidFlushReceiveBuffer();
}

// UART initialization with interrupt support
void UART_voidInitWithInterrupt(void) {
    // Initialize UART normally first
    UART_voidInit();
    
    // Enable RX Complete Interrupt
    UCSRB |= (1 << RXCIE);
    
    // Enable global interrupts
    sei();
}

// Send single byte
void UART_voidSendByte(u8 u8Data) {
    // Wait for empty transmit buffer
    while (!(UCSRA & (1 << UDRE)));
    
    // Put data into buffer, sends the data
    UDR = u8Data;
}

// Send single character (alias for SendByte)
void UART_voidSendChar(char c) {
    UART_voidSendByte((u8)c);
}

// Send string
void UART_voidSendString(const char* str) {
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

// Send packet of data
void UART_voidSendPacket(u8 *data, u16 length) {
    for (u16 i = 0; i < length; i++) {
        UART_voidSendByte(data[i]);
    }
}

// Blocking receive of single byte
u8 UART_u8ReceiveByte(void) {
    // Wait for data to be received
    while (!(UCSRA & (1 << RXC)));
    
    // Get and return received data from buffer
    return UDR;
}

// Non-blocking receive of single byte
u8 UART_u8ReceiveByteNonBlocking(u8 *data) {
    // Disable interrupts temporarily
    cli();
    
    // Check if data is available in buffer
    if (UART_RxHead != UART_RxTail) {
        *data = UART_RxBuffer[UART_RxTail];
        UART_RxTail = (UART_RxTail + 1) % UART_BUFFER_SIZE;
        
        // Update flag if buffer becomes empty
        if (UART_RxHead == UART_RxTail) {
            UART_RxFlag = 0;
        }
        
        sei(); // Re-enable interrupts
        return 1; // Data received
    }
    
    sei(); // Re-enable interrupts
    return 0; // No data available
}

// Check if data is available in receive buffer
u8 UART_u8DataAvailable(void) {
    return (UART_RxHead != UART_RxTail) ? 1 : 0;
}

// Flush the receive buffer
void UART_voidFlushReceiveBuffer(void) {
    cli(); // Disable interrupts
    UART_RxHead = 0;
    UART_RxTail = 0;
    UART_RxFlag = 0;
    sei(); // Re-enable interrupts
}

// Calculate checksum for data packet
u8 UART_u8CalculateChecksum(u8 *data, u16 length) {
    u16 i;
    u8 checksum = 0;
    
    for (i = 0; i < length; i++) {
        checksum += data[i];
    }
    
    return checksum;
}
