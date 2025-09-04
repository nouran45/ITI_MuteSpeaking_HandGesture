#include "../src/LIB/STD_TYPES.h"
#include "../src/LIB/BIT_MATH.h"
#include "../src/MCAL/DIO/DIO_Interface.h"
#include "../src/HAL/LCD/LCD_Interface.h"
#include <util/delay.h>

int main(void) {
    u16 counter = 0;
    
    // Initialize peripherals
    DIO_voidInit();
    LCD_vidInit_8bits();  // Using 8-bit mode to match hardware connections
    
    // Show LCD is working
    LCD_vidWriteString((u8*)"LCD Test Mode");
    LCD_vidGotoxy(0, 1);
    LCD_vidWriteString((u8*)"Counter: ");
    _delay_ms(2000);
    
    while(1) {
        // Update counter display
        LCD_vidGotoxy(9, 1);  // Position after "Counter: "
        LCD_vidWriteNumber_8bits(counter);
        LCD_vidWriteString((u8*)"   ");  // Clear any leftover digits
        
        counter++;
        if (counter > 9999) counter = 0;  // Reset after 9999
        
        _delay_ms(100);  // Update every 100ms
    }
    
    return 0;
}
