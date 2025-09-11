/***************************************************************************************/
/****************************  IMT School Training Center ******************************/
/***************************************************************************************/
/** This file is developed by IMT School training center, All copyrights are reserved **/
/***************************************************************************************/
/* Description ! 16x2 Character LCD for chip LMB161A                                   */
/* Release Date! 28/6/2016                                                             */
/* Version     ! V01                                                                   */
/***************************************************************************************/
#ifndef LCD_H_
#define LCD_H_

/***************************************************************************************/
/********************************** Function Definition ********************************/
/***************************************************************************************/
#include "../../LIB/STD_TYPES.h" 


/***************************************************************************************/
/* Description! Apply initialization sequence for LCD module (4-bit mode)             */
/* Input      ! Nothing                                                                */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidInit_4bits(void);

/***************************************************************************************/
/* Description! Helper function to send 4 bits to LCD                                 */
/* Input      ! 4-bit data                                                             */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_Send4Bits(u8 data);

/***************************************************************************************/
/* Description! Apply initialization sequence for LCD module (8-bit mode - deprecated)*/
/* Input      ! Nothing                                                                */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidInit_8bits(void);

/***************************************************************************************/
/* Description! Interface to send the configuration commands to the LCD Driver         */
/* Input      ! Command number                                                         */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidSendCommand(u8 u8CmdCpy);

void LCD_vidWriteNumber_4bits(u16 Copy_u16Number);
void LCD_vidWriteNumber_8bits(u16 Copy_u16Number);
/***************************************************************************************/
/* Description! Interface to write character on LCD screen                             */
/* Input      ! Data to send                                                           */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidWriteCharctr(u8 u8DataCpy);

/***************************************************************************************/
/* Description! Interface to write string on LCD screen                                */
/* Input      ! Pointer to the string                                                  */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidWriteString (u8* pu8StringCpy);
/***************************************************************************************/
/* Description! Interface to start the writing process at certain digit in the LCD     */
/* Input      ! Y = Row position , X = Column position                                 */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidGotoxy(u8 X, u8 Y);
/***************************************************************************************/
/* Description! Interface to write extra characters saved in the CGRAM                 */
/* Input      ! Y = Row position , X = Column position                                 */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidWriteExtraChar (u8 Y,u8 X);
/***************************************************************************************/
/* Description! Interface to write a number on LCD as a string                         */
/* Input      ! Copy_u16Number = Number needs to be written on the LCD                 */
/* Output     ! Nothing                                                                */
/***************************************************************************************/
void LCD_vidWriteNumber(u16 Copy_u16Number);

void LCD_vidInit_4bits(void);

void LCD_vidWriteCharctr_4bits(u8 u8DataCpy);

void LCD_vidSendCommand_(u8 u8Cmd);
void LCD_vidSendCommand_4_Bits(u8 u8CmdCpy);
void LCD_Char( unsigned char data );
void LCD_Init (void);
void LCD_Command( unsigned char cmnd );
void LCD_Clear(void);
void LCD_voidSendString(char * ARG_charPtr );
void LCD_vidWriteSignedNumber(s16 s16Number);
// Add this function prototype
void LCD_vidWriteString(u8* pu8StringCpy);
#endif /* LCD_H_ */
