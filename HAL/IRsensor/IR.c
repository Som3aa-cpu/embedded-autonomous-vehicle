/*
 * IR.c
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */


#include "IR_interface.h"
#include "IR_priv.h"
#include "IR_config.h"
#include "../../MCAL/DIO/DIO_interface.h"
#include "../../Bit_math.h"


void IRSensor_voidInit(void)
{
    DIO_setPinDirection(IRSENSOR_PORT, IRSENSOR_CH0_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IRSENSOR_PORT, IRSENSOR_CH1_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IRSENSOR_PORT, IRSENSOR_CH2_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IRSENSOR_PORT, IRSENSOR_CH3_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IRSENSOR_PORT, IRSENSOR_CH4_PIN, DIO_PIN_INPUT);
}

uint8_t IRSensor_u8ReadChannel(uint8_t Copy_u8Channel)
{
    uint8_t Local_u8BitState = 0;

    switch (Copy_u8Channel)
    {
        case IRSENSOR_CHANNEL_0:
            DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH0_PIN, &Local_u8BitState);
            break;
        case IRSENSOR_CHANNEL_1:
            DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH1_PIN, &Local_u8BitState);
            break;
        case IRSENSOR_CHANNEL_2:
            DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH2_PIN, &Local_u8BitState);
            break;
        case IRSENSOR_CHANNEL_3:
            DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH3_PIN, &Local_u8BitState);
            break;
        case IRSENSOR_CHANNEL_4:
            DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH4_PIN, &Local_u8BitState);
            break;
        default:
            break;
    }

    return Local_u8BitState;
}

uint8_t IRSensor_u8ReadRaw(void)
{
    uint8_t Local_u8RawData = 0;
    uint8_t Local_u8PinVal  = 0;

    DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH0_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 0);

    DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH1_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 1);

    DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH2_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 2);

    DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH3_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 3);

    DIO_getPinValue(IRSENSOR_PORT, IRSENSOR_CH4_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 4);

    return Local_u8RawData;
}
