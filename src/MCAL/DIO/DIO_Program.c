#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "DIO_Register.h"
void DIO_voidInit(void) {
    // Initialize all port output values to 0
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    // Set all port pins as inputs
    DDRA = 0x00;
    DDRB = 0x00;
    // Special handling for PORTC due to I2C functionality
    DDRC = 0x00;  // I2C pins should be inputs with pull-ups
    DDRD = 0x00;
}

void DIO_voidSetPortDirection(u8 u8PortId, u8 Direction)
{

		switch(u8PortId){
			case DIO_u8_PORTA : DDRA= 0b00001111; break;
			case DIO_u8_PORTB : DDRB= 0b00001111; break;
			case DIO_u8_PORTC : DDRC= 0b00001111; break;
			case DIO_u8_PORTD : DDRD= 0b00001111; break;
		}

}
void DIO_voidSetPortValue(u8 u8PortId, u8 u8PortVal){
		
	switch(u8PortId){
		case DIO_u8_PORTA :PORTA=u8PortVal;  break;
		case DIO_u8_PORTB :PORTB=u8PortVal; break;
		case DIO_u8_PORTC :PORTC=u8PortVal; break;
		case DIO_u8_PORTD :PORTD=u8PortVal;  break;
	}
}
u8 DIO_u8GetPinValue(u8 u8PortIdCopy, u8 u8PinIdCopy){
	u8 value = 0;
	switch(u8PortIdCopy){
		case DIO_u8_PORTA : value = GET_BIT(PINA , u8PinIdCopy); break;
		case DIO_u8_PORTB : value = GET_BIT(PINB , u8PinIdCopy); break;
		case DIO_u8_PORTC : value = GET_BIT(PINC , u8PinIdCopy); break;
		case DIO_u8_PORTD : value = GET_BIT(PIND , u8PinIdCopy); break;
	}
	return value;
}

void DIO_voidSetPinValue(u8 u8PortIdCopy, u8 u8PinIdCopy, u8 u8PinValCopy){
	if(u8PinValCopy == DIO_u8_HIGH)
	{
		switch(u8PortIdCopy){
			case DIO_u8_PORTA : SET_BIT(PORTA , u8PinIdCopy); break;
			case DIO_u8_PORTB : SET_BIT(PORTB , u8PinIdCopy); break;
			case DIO_u8_PORTC : SET_BIT(PORTC , u8PinIdCopy); break;
			case DIO_u8_PORTD : SET_BIT(PORTD , u8PinIdCopy); break;
		}
	}else{
		switch(u8PortIdCopy){
			case DIO_u8_PORTA : CLR_BIT(PORTA , u8PinIdCopy); break;
			case DIO_u8_PORTB : CLR_BIT(PORTB , u8PinIdCopy); break;
			case DIO_u8_PORTC : CLR_BIT(PORTC , u8PinIdCopy); break;
			case DIO_u8_PORTD : CLR_BIT(PORTD , u8PinIdCopy); break;
		}

	}

}
void DIO_voidSetPinDirection(u8 u8PortIdCopy, u8 u8PinIdCopy, u8 u8PinDirCopy){
	if(u8PinDirCopy == DIO_u8_OUTPUT)
	{
		switch(u8PortIdCopy){
			case DIO_u8_PORTA : SET_BIT(DDRA , u8PinIdCopy); break;
			case DIO_u8_PORTB : SET_BIT(DDRB , u8PinIdCopy); break;
			case DIO_u8_PORTC : SET_BIT(DDRC , u8PinIdCopy); break;
			case DIO_u8_PORTD : SET_BIT(DDRD , u8PinIdCopy); break;
		}
	}else{
		switch(u8PortIdCopy){
			case DIO_u8_PORTA : CLR_BIT(DDRA , u8PinIdCopy); break;
			case DIO_u8_PORTB : CLR_BIT(DDRB , u8PinIdCopy); break;
			case DIO_u8_PORTC : CLR_BIT(DDRC , u8PinIdCopy); break;
			case DIO_u8_PORTD : CLR_BIT(DDRD , u8PinIdCopy); break;
		}

	}

}



