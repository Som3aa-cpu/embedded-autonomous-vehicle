/*
 * DIO_prog.c
 *
 *  Created on: 19 Jul 2026
 *      Author: mohib
 */
#include "DIO_REG.h"
#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "DIO_interface.h"


void DIO_setPinDirection(uint8_t PortId, uint8_t pinId, uint8_t Pindirection)
{
	if ((PortId <=3)&& pinId <= 7 && (DIO_PIN_OUTPUT==Pindirection || DIO_PIN_INPUT==Pindirection))
	{
		switch(PortId)
		{
		case DIO_PORTA:
			if (DIO_PIN_OUTPUT == Pindirection)
			{
				Set_Bit(DDRA_Register, pinId);
			}
			else
			{
				Clr_Bit(DDRA_Register,pinId);
			}break;
		case DIO_PORTB:
			if (DIO_PIN_OUTPUT == Pindirection)
			{
				Set_Bit(DDRB_Register, pinId);
			}
			else
			{
				Clr_Bit(DDRB_Register,pinId);
			}break;
		case DIO_PORTC:
			if (DIO_PIN_OUTPUT == Pindirection)
			{
				Set_Bit(DDRC_Register, pinId);
			}
			else
			{
				Clr_Bit(DDRC_Register,pinId);
			}break;
		case DIO_PORTD:
			if (DIO_PIN_OUTPUT == Pindirection)
			{
				Set_Bit(DDRD_Register, pinId);
			}
			else
			{
				Clr_Bit(DDRD_Register,pinId);
			}break;

		}
	}

}
void DIO_setPinValue(uint8_t PortId, uint8_t pinId, uint8_t PinValue)
{
	switch(PortId)
	{
	case DIO_PORTA:
		if (DIO_PIN_HIGH == PinValue)
		{
			Set_Bit(PORTA_Register, pinId);
		}
		else
		{
			Clr_Bit(PORTA_Register, pinId);
		}break;
	case DIO_PORTB:
		if (DIO_PIN_HIGH == PinValue)
		{
			Set_Bit(PORTB_Register, pinId);
		}
		else
		{
			Clr_Bit(PORTB_Register, pinId);
		}break;
	case DIO_PORTC:
		if (DIO_PIN_HIGH == PinValue)
		{
			Set_Bit(PORTC_Register, pinId);
		}
		else
		{
			Clr_Bit(PORTC_Register, pinId);
		}break;
	case DIO_PORTD:
		if (DIO_PIN_HIGH == PinValue)
		{
			Set_Bit(PORTD_Register, pinId);
		}
		else
		{
			Clr_Bit(PORTD_Register, pinId);
		}break;
	}
}
void DIO_getPinValue(uint8_t PortId, uint8_t pinId, uint8_t* PinValue)
{
	switch(PortId)
	{
	case DIO_PORTA:
		*PinValue = Get_Bit(PINA_Register, pinId);
		break;
	case DIO_PORTB:
		*PinValue = Get_Bit(PINB_Register, pinId);
		break;
	case DIO_PORTC:
		*PinValue = Get_Bit(PINC_Register, pinId);
		break;
	case DIO_PORTD:
		*PinValue = Get_Bit(PIND_Register, pinId);
		break;
	}
}
void DIO_togglePinValues(uint8_t PortId, uint8_t pinId)
{
	switch(PortId)
	{
	case DIO_PORTA:
		Toggle_Bit(PORTA_Register, pinId);
		break;
	case DIO_PORTB:
		Toggle_Bit(PORTB_Register, pinId);
		break;
	case DIO_PORTC:
		Toggle_Bit(PORTC_Register, pinId);
		break;
	case DIO_PORTD:
		Toggle_Bit(PORTD_Register, pinId);
		break;
	}
}
void DIO_activePullUpRessistencePin(uint8_t PortId, uint8_t pinId)
{
	switch(PortId)
	{
	case DIO_PORTA:
		Set_Bit(PORTA_Register, pinId);
		break;
	case DIO_PORTB:
		Set_Bit(PORTB_Register, pinId);
		break;
	case DIO_PORTC:
		Set_Bit(PORTC_Register, pinId);
		break;
	case DIO_PORTD:
		Set_Bit(PORTD_Register, pinId);
		break;
	}
}
void DIO_setPortDirection(uint8_t PortId,  uint8_t PortDirection)
{
	if ((PortId <= 3) && ((PortDirection == DIO_PORT_OUTPUT) || (PortDirection == DIO_PORT_INPUT)))
	{
		switch(PortId)
		{
		case DIO_PORTA:
			if (DIO_PORT_OUTPUT == PortDirection)
			{
				DDRA_Register = 0xFF;
			}
			else
			{
				DDRA_Register = 0x00;
			}
			break;
		case DIO_PORTB:
			if (DIO_PORT_OUTPUT == PortDirection)
			{
				DDRB_Register = 0xFF;
			}
			else
			{
				DDRB_Register = 0x00;
			}
			break;
		case DIO_PORTC:
			if (DIO_PORT_OUTPUT == PortDirection)
			{
				DDRC_Register = 0xFF;
			}
			else
			{
				DDRC_Register = 0x00;
			}
			break;
		case DIO_PORTD:
			if (DIO_PORT_OUTPUT == PortDirection)
			{
				DDRD_Register = 0xFF;
			}
			else
			{
				DDRD_Register = 0x00;
			}
			break;
		}
	}
}
void DIO_setPortValue(uint8_t PortId, uint8_t PortValue)
{
	if(PortId<=3)
	{
		switch(PortId)
		{
		case DIO_PORTA:
			PORTA_Register = PortValue;
			break;
		case DIO_PORTB:
			PORTB_Register = PortValue;
			break;
		case DIO_PORTC:
			PORTC_Register = PortValue;
			break;
		case DIO_PORTD:
			PORTD_Register = PortValue;
			break;
		}
	}
}
void DIO_getPortValue(uint8_t PortId, uint8_t* PortValue)
{
	if (PortId<=3 && PortValue != 0)
	{
		switch(PortId)
		{
		case DIO_PORTA:
			*PortValue = PINA_Register;
			break;
		case DIO_PORTB:
			*PortValue = PINB_Register;
			break;
		case DIO_PORTC:
			*PortValue = PINC_Register;
			break;
		case DIO_PORTD:
			*PortValue = PIND_Register;
			break;
		}
	}
}
void DIO_togglePortValues(uint8_t PortId)
{
	if(PortId<=3)
	{
		switch(PortId)
		{
		case DIO_PORTA:
			PORTA_Register=~PORTA_Register;
			break;
		case DIO_PORTB:
			PORTB_Register=~PORTB_Register;
			break;
		case DIO_PORTC:
			PORTC_Register=~PORTC_Register;
			break;
		case DIO_PORTD:
			PORTD_Register=~PORTD_Register;
			break;
		}
	}
}
void DIO_activePullUpRessistencePort(uint8_t PortId)
{
	if(PortId<=3)
	{
		switch(PortId)
		{
		case DIO_PORTA:
			PORTA_Register=0xFF;
			break;
		case DIO_PORTB:
			PORTB_Register=0xFF;
			break;
		case DIO_PORTC:
			PORTC_Register=0xFF;
			break;
		case DIO_PORTD:
			PORTD_Register=0xFF	;
			break;
		}
	}
}

