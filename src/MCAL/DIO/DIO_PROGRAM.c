#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "DIO_Register.h"


void DIO_voidSetPortDirection(u8 u8PortId, u8 Direction) {
    if(Direction == DIO_u8_OUTPUT) {
        switch(u8PortId) {
            case DIO_u8_PORTA: DDRA = 0xFF; break;
            case DIO_u8_PORTB: DDRB = 0xFF; break;
            case DIO_u8_PORTC: DDRC = 0xFF; break;
            case DIO_u8_PORTD: DDRD = 0xFF; break;
        }
    } else if(Direction == DIO_u8_INPUT) {
        switch(u8PortId) {
            case DIO_u8_PORTA: DDRA = 0x00; break;
            case DIO_u8_PORTB: DDRB = 0x00; break;
            case DIO_u8_PORTC: DDRC = 0x00; break;
            case DIO_u8_PORTD: DDRD = 0x00; break;
        }
    }
}




 void DIO_voidSetPortValue(u8 u8PortId, u8 u8PortVal){


        switch(u8PortId) {
            case DIO_u8_PORTA: PORTA = u8PortVal; break;
            case DIO_u8_PORTB: PORTB = u8PortVal; break;
            case DIO_u8_PORTC: PORTC = u8PortVal; break;
            case DIO_u8_PORTD: PORTD = u8PortVal; break;

        }

}


void DIO_voidSetPinValue(u8 u8PortIdCopy , u8 u8PinIdCopy, u8 u8PinValCopy){



    if(u8PinValCopy) {
        switch(u8PortIdCopy) {
            case DIO_u8_PORTA: SET_BIT(PORTA, u8PinIdCopy); break;
            case DIO_u8_PORTB: SET_BIT(PORTB, u8PinIdCopy); break;
            case DIO_u8_PORTC: SET_BIT(PORTC, u8PinIdCopy); break;
            case DIO_u8_PORTD: SET_BIT(PORTD, u8PinIdCopy); break;
        }
    } else if(!u8PinValCopy) {
        switch(u8PortIdCopy) {
            case DIO_u8_PORTA: CLR_BIT(PORTA, u8PinIdCopy); break;
            case DIO_u8_PORTB: CLR_BIT(PORTB, u8PinIdCopy); break;
            case DIO_u8_PORTC: CLR_BIT(PORTC, u8PinIdCopy); break;
            case DIO_u8_PORTD: CLR_BIT(PORTD, u8PinIdCopy); break;
        }
    }




}

u8   DIO_u8GetPinValue(u8 u8PortIdCopy, u8 u8PinIdCopy){
    u8 Local_u8PinVal = 0;

    switch(u8PortIdCopy) {
        case DIO_u8_PORTA: Local_u8PinVal = GET_BIT(PINA, u8PinIdCopy); break;
        case DIO_u8_PORTB: Local_u8PinVal = GET_BIT(PINB, u8PinIdCopy); break;
        case DIO_u8_PORTC: Local_u8PinVal = GET_BIT(PINC, u8PinIdCopy); break;
        case DIO_u8_PORTD: Local_u8PinVal = GET_BIT(PIND, u8PinIdCopy); break;
    }
    return Local_u8PinVal;


}

void DIO_voidSetPinDirection(u8 u8PortIdCopy, u8 u8PinIdCopy, u8 u8PinDirCopy){

    if(u8PinDirCopy) {
        switch(u8PortIdCopy) {
            case DIO_u8_PORTA: SET_BIT(DDRA, u8PinIdCopy); break;
            case DIO_u8_PORTB: SET_BIT(DDRB, u8PinIdCopy); break;
            case DIO_u8_PORTC: SET_BIT(DDRC, u8PinIdCopy); break;
            case DIO_u8_PORTD: SET_BIT(DDRD, u8PinIdCopy); break;
        }
    } else if(!u8PinDirCopy) {
        switch(u8PortIdCopy) {
            case DIO_u8_PORTA: CLR_BIT(DDRA, u8PinIdCopy); break;
            case DIO_u8_PORTB: CLR_BIT(DDRB, u8PinIdCopy); break;
            case DIO_u8_PORTC: CLR_BIT(DDRC, u8PinIdCopy); break;
            case DIO_u8_PORTD: CLR_BIT(DDRD, u8PinIdCopy); break;
        }
    }
}



void DIO_voidInit(void) {

    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    DDRA = 0x00;
    DDRB = 0x00;
    // Don't configure PORTC direction - let I2C handle PC0/PC1
    // PC0 = SDA, PC1 = SCL for ATmega32a I2C
    DDRC = 0x00;  // I2C pins should be inputs with pull-ups
    DDRD = 0x00;
}