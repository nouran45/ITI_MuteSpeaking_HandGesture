#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "LCD_Config.h"
#include "LCD_Interface.h"
#include "DIO_Register.h"
#include <util/delay.h>
#include <stdio.h>  // For sprintf function

// 4-bit mode LCD initialization - frees up PC0 for I2C
void LCD_vidInit_4bits(void) {
    // Set control pins as outputs - NEW pins to avoid I2C conflict
    DIO_voidSetPinDirection(DIO_u8_PORTA, 0, DIO_u8_OUTPUT); // RS = PA0
    DIO_voidSetPinDirection(DIO_u8_PORTA, 1, DIO_u8_OUTPUT); // E = PA1
    
    // Set only upper 4 bits of PORTD as output (PD4-PD7)
    DIO_voidSetPinDirection(DIO_u8_PORTD, 4, DIO_u8_OUTPUT); // D4 = PD4
    DIO_voidSetPinDirection(DIO_u8_PORTD, 5, DIO_u8_OUTPUT); // D5 = PD5
    DIO_voidSetPinDirection(DIO_u8_PORTD, 6, DIO_u8_OUTPUT); // D6 = PD6
    DIO_voidSetPinDirection(DIO_u8_PORTD, 7, DIO_u8_OUTPUT); // D7 = PD7
    
    // Wait for LCD to power up
    _delay_ms(100);
    
    // 4-bit initialization sequence
    // RS = 0 for command
    DIO_voidSetPinValue(DIO_u8_PORTA, 0, DIO_u8_LOW);
    
    // 4-bit initialization sequence
    // Send function set command multiple times (3x) for reliable initialization
    // First three sends are in 8-bit mode to get LCD's attention
    
    // Send 0x30 three times (8-bit mode command) 
    LCD_Send4Bits(0x3);
    _delay_ms(5);
    LCD_Send4Bits(0x3);
    _delay_ms(1);
    LCD_Send4Bits(0x3);
    _delay_ms(1);
    
    // Now switch to 4-bit mode
    LCD_Send4Bits(0x2);  // 4-bit mode
    _delay_ms(1);
    
    // Function Set: 4-bit, 2 line, 5x8 font (0x28)
    LCD_vidSendCommand(0x28);
    
    // Display OFF (0x08)
    LCD_vidSendCommand(0x08);
    
    // Clear display (0x01)
    LCD_vidSendCommand(0x01);
    _delay_ms(2);
    
    // Entry mode: increment, no shift (0x06)
    LCD_vidSendCommand(0x06);
    
    // Display ON, cursor OFF (0x0C)
    LCD_vidSendCommand(0x0C);
}

// Helper function to send 4 bits on PD4-PD7
void LCD_Send4Bits(u8 data) {
    // Clear the upper 4 bits of PORTD first
    PORTD &= 0x0F;  // Keep lower 4 bits, clear upper 4 bits
    
    // Set the upper 4 bits with our data
    PORTD |= (data << 4);  // Shift data to upper 4 bits and OR with existing
    
    // Generate enable pulse on PA1
    DIO_voidSetPinValue(DIO_u8_PORTA, 1, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTA, 1, DIO_u8_LOW);
    _delay_us(50);  // Give LCD time to process
}

// Command function for 4-bit mode
void LCD_vidSendCommand(u8 u8CmdCpy) {
    // Set RS low for command mode (PA0)
    DIO_voidSetPinValue(DIO_u8_PORTA, 0, DIO_u8_LOW);
    
    // Send upper 4 bits first
    LCD_Send4Bits((u8CmdCpy >> 4) & 0x0F);
    
    // Send lower 4 bits
    LCD_Send4Bits(u8CmdCpy & 0x0F);
    
    _delay_ms(2);  // Give LCD time to process command
}

// Character write function for 4-bit mode
void LCD_vidWriteCharctr(u8 u8DataCpy) {
    // Set RS high for data mode (PA0)
    DIO_voidSetPinValue(DIO_u8_PORTA, 0, DIO_u8_HIGH);
    
    // Send upper 4 bits first
    LCD_Send4Bits((u8DataCpy >> 4) & 0x0F);
    
    // Send lower 4 bits
    LCD_Send4Bits(u8DataCpy & 0x0F);
    
    _delay_ms(1);  // Give LCD time to process data
}


void LCD_vidGotoxy(u8 X, u8 Y)
{
	// Validate coordinates
	if (X < 16 && Y < 2)
	{
		// Calculate DDRAM address
		u8 address = (Y == 0) ? (0x80 + X) : (0xC0 + X);
		LCD_vidSendCommand(address);
	}
}


void LCD_Clear()
{
	// Clear display - command 0x01
	LCD_vidSendCommand(0x01);		
	_delay_ms(2);  // This command needs extra time
	
	// Return home - command 0x02 or set cursor position to 0,0
	LCD_vidSendCommand(0x80);  // First position first line
}


void LCD_vidWriteNumber_8bits(u16 Copy_u16Number)
{
    char buffer[10];
    u8 i = 0;

    if (Copy_u16Number == 0)
    {
    	LCD_vidWriteCharctr('0');
        return;
    }
    
    while (Copy_u16Number > 0)
    {
        buffer[i] = (Copy_u16Number % 10) + '0';
        Copy_u16Number /= 10;
        i++;
    }

    while (i > 0)
    {
        i--;
        LCD_vidWriteCharctr(buffer[i]);
    }
}

// Add missing LCD_vidWriteNumber function (wrapper for compatibility)
void LCD_vidWriteNumber(u16 Copy_u16Number)
{
    LCD_vidWriteNumber_8bits(Copy_u16Number);
}





void LCD_Command(unsigned char cmnd)
{
    // Just forward to our 8-bit command function for consistency
    LCD_vidSendCommand(cmnd);
}

void LCD_Char(unsigned char data)
{
    // Just forward to our 8-bit data function for consistency
    LCD_vidWriteCharctr(data);
}

void LCD_Init(void)
{
    // Forward to our new 4-bit initialization function
    LCD_vidInit_4bits();
}


void LCD_voidSendString(char * ARG_charPtr )
{
    u8 i = 0;
    if (ARG_charPtr == 0)
    {
        
        return;
    }
    else { 
        do {
            LCD_Char(ARG_charPtr[i]);
            i++;
        } while(ARG_charPtr[i] != '\0');
    }
}


void LCD_vidWriteSignedNumber(s16 s16Number) {
    if (s16Number < 0) {
        LCD_vidWriteCharctr('-');
        s16Number = -s16Number;
    } else {
        LCD_vidWriteCharctr(' ');
    }
    LCD_vidWriteNumber_8bits((u16)s16Number);
}


// Add this function
void LCD_vidWriteString(u8* pu8StringCpy) {
    u8 i = 0;
    while (pu8StringCpy[i] != '\0') {
        LCD_vidWriteCharctr(pu8StringCpy[i]);
        i++;
    }
}