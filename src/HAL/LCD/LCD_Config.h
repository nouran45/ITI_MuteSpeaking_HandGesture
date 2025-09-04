//**************************************************************************************/
#ifndef LCD_CONFIG_H_
#define LCD_CONFIG_H_

/****************************   LCD PIN Configuration  *********************************/

#define LCD_DATA_PORT              DIO_u8_PORTD
#define LCD_CONTROL_PORT           DIO_u8_PORTC
#define LCD_RS_PIN                 DIO_u8_PIN0
#define LCD_RW_PIN                 DIO_u8_PIN1
#define LCD_E_PIN                  DIO_u8_PIN2

#define LCD_8_BITS                   0
#define LCD_4_BitS                   1
#define LCD_I2C                      2

///0->1 line 1->2 lines
///0->5*8  1->5*11
#define Function_Set_8_Bits 0b00110000

#define Function_Set_4_Bits_1 0b00100000

#define Function_Set_4_Bits_2 0b00100000

#define Function_Set_4_Bits_3 0b1100000

//d display       1->enable display
//c cursor enable 1->display cursor
//b blink cursor  1-> blink cursor

#define Display_ON_OFF 0b00001111
#define Display_ON_OFF_ 0b00001100
#define Clear_LCD      0b00000001

#define Entry_Mode 0b00000111
#define LCD_Dir  DDRD			/* Define LCD data port direction */
#define LCD_Port PORTD			/* Define LCD data port */
#define RS DIO_u8_PIN0				/* Define Register Select pin */
#define EN DIO_u8_PIN2				/* Define Enable signal pin */

#endif /* LCD_CONFIG_H_ */
