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


void IR_voidInit(void) {
    /* Set all 5 sensor pins as INPUT via DIO driver */
    DIO_setPinDirection(IR_CH1_PORT, IR_CH1_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IR_CH2_PORT, IR_CH2_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IR_CH3_PORT, IR_CH3_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IR_CH4_PORT, IR_CH4_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(IR_CH5_PORT, IR_CH5_PIN, DIO_PIN_INPUT);
}

uint8_t IR_u8ReadChannel(uint8_t Copy_u8Channel) {
    uint8_t Local_u8BitState = 0;

    switch (Copy_u8Channel) {
        case IR_CHANNEL_1: 
            DIO_getPinValue(IR_CH1_PORT, IR_CH1_PIN, &Local_u8BitState); 
            break;
        case IR_CHANNEL_2: 
            DIO_getPinValue(IR_CH2_PORT, IR_CH2_PIN, &Local_u8BitState); 
            break;
        case IR_CHANNEL_3: 
            DIO_getPinValue(IR_CH3_PORT, IR_CH3_PIN, &Local_u8BitState); 
            break;
        case IR_CHANNEL_4: 
            DIO_getPinValue(IR_CH4_PORT, IR_CH4_PIN, &Local_u8BitState); 
            break;
        case IR_CHANNEL_5: 
            DIO_getPinValue(IR_CH5_PORT, IR_CH5_PIN, &Local_u8BitState); 
            break;
        default: 
            break;
    }

    return Local_u8BitState;
}

uint8_t IR_u8ReadRaw(void) {
    uint8_t Local_u8RawData = 0;
    uint8_t Local_u8PinVal  = 0;

    DIO_getPinValue(IR_CH1_PORT, IR_CH1_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 0);

    DIO_getPinValue(IR_CH2_PORT, IR_CH2_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 1);

    DIO_getPinValue(IR_CH3_PORT, IR_CH3_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 2);

    DIO_getPinValue(IR_CH4_PORT, IR_CH4_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 3);

    DIO_getPinValue(IR_CH5_PORT, IR_CH5_PIN, &Local_u8PinVal);
    if (Local_u8PinVal) Set_Bit(Local_u8RawData, 4);

    return Local_u8RawData;
}
