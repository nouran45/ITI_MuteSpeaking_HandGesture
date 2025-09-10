#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "DIO_Register.h"
#include "LCD_Interface.h"
#include "LCD_Config.h"

#include <util/delay.h>
/*
 * This is a diagnostic program for testing LCD display functionality.
 * To use this instead of main.c, compile with:
 * avr-gcc -Os -DF_CPU=8000000UL -mmcu=atmega32a -o LCD_diagnostic.elf LCD_diagnostic.c DIO_PROGRAM.c LCD_Program.c
 * avr-objcopy -O ihex -R .eeprom LCD_diagnostic.elf LCD_diagnostic.hex
 * avrdude -c usbasp -p atmega32a -U flash:w:LCD_diagnostic.hex
 */

int main(void) {
    // Initialize DIO
    DIO_voidInit();
    
    // Initialize the LCD in 8-bit mode
    // Set control pins as outputs
    DIO_voidSetPinDirection(DIO_u8_PORTC, 0, DIO_u8_OUTPUT); // RS = PC0
    DIO_voidSetPinDirection(DIO_u8_PORTC, 2, DIO_u8_OUTPUT); // E = PC2
    
    // Set data port as output (PORTD = PD0-PD7)
    DIO_voidSetPortDirection(DIO_u8_PORTD, DIO_u8_OUTPUT);
    
    // Wait for LCD to power up
    _delay_ms(100);
    
    // Manual initialization sequence
    // RS = 0 for command
    DIO_voidSetPinValue(DIO_u8_PORTC, 0, DIO_u8_LOW);
    
    // Send 0x30 (8-bit mode, 1 line, 5x8 font) multiple times for initialization
    DIO_voidSetPortValue(DIO_u8_PORTD, 0x30);
    
    // Enable pulse
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(5);
    
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(1);
    
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(1);
    
    // Set 8-bit, 2 line, 5x8 font
    DIO_voidSetPortValue(DIO_u8_PORTD, 0x38);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(1);
    
    // Display OFF
    DIO_voidSetPortValue(DIO_u8_PORTD, 0x08);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(1);
    
    // Clear display
    DIO_voidSetPortValue(DIO_u8_PORTD, 0x01);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(2);
    
    // Entry mode: increment, no shift
    DIO_voidSetPortValue(DIO_u8_PORTD, 0x06);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(1);
    
    // Display ON, cursor ON, blink ON
    DIO_voidSetPortValue(DIO_u8_PORTD, 0x0F);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(1);
    
    // Writing a simple test pattern
    // Set RS high for data mode
    DIO_voidSetPinValue(DIO_u8_PORTC, 0, DIO_u8_HIGH);
    
    // Write 'TEST' to LCD
    char testString[] = "LCD TEST";
    for (int i = 0; testString[i] != '\0'; i++) {
        // Send the character
        DIO_voidSetPortValue(DIO_u8_PORTD, testString[i]);
        
        // Enable pulse
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
        _delay_us(1);
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
        _delay_ms(1);
    }
    
    // Move to second line
    DIO_voidSetPinValue(DIO_u8_PORTC, 0, DIO_u8_LOW); // RS = 0 for command
    DIO_voidSetPortValue(DIO_u8_PORTD, 0xC0); // Second line address
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
    _delay_us(1);
    DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
    _delay_ms(1);
    
    // Write '12345678' to second line
    DIO_voidSetPinValue(DIO_u8_PORTC, 0, DIO_u8_HIGH); // RS = 1 for data
    char numberString[] = "12345678";
    for (int i = 0; numberString[i] != '\0'; i++) {
        // Send the character
        DIO_voidSetPortValue(DIO_u8_PORTD, numberString[i]);
        
        // Enable pulse
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
        _delay_us(1);
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
        _delay_ms(1);
    }
    
    // Infinite loop to hold the display
    while(1) {
        // Toggle the cursor every second to show the system is still running
        DIO_voidSetPinValue(DIO_u8_PORTC, 0, DIO_u8_LOW); // RS = 0 for command
        
        // Display ON, cursor OFF
        DIO_voidSetPortValue(DIO_u8_PORTD, 0x0C);
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
        _delay_us(1);
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
        _delay_ms(500);
        
        // Display ON, cursor ON
        DIO_voidSetPortValue(DIO_u8_PORTD, 0x0E);
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_HIGH);
        _delay_us(1);
        DIO_voidSetPinValue(DIO_u8_PORTC, 2, DIO_u8_LOW);
        _delay_ms(500);
    }
    
    return 0;
}
