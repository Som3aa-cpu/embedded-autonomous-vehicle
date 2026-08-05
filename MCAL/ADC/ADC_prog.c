/*
 * ADC_prog.c
 *
 *  Created on: 27 Jul 2026
 *      Author: mohib
 */

#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "../ADC/ADC_interface.h"
#include "../ADC/ADC_Reg.h"

void ADC_init(void)
{
    // 1. select Vref = AVCC
    Set_Bit(ADC_ADMUX, 6); 
    Clr_Bit(ADC_ADMUX, 7); 

    // 2. Right Adjust 
    Clr_Bit(ADC_ADMUX, 5); 

    // 3. Disable autotrigger
    Clr_Bit(ADC_ADCSRA, 5); 

    // 4. Select clk/64
    Set_Bit(ADC_ADCSRA, 2);
    Set_Bit(ADC_ADCSRA, 1);
    Clr_Bit(ADC_ADCSRA, 0);
    // 5. Enable ADC
    Set_Bit(ADC_ADCSRA, 7); 
}

uint8_t ADC_GetDigitalValue(uint8_t Copy_channel, uint16_t *Copy_digitalValue)
{
    uint8_t Local_errorCode = StdTypes_OK;

    if((Copy_channel <= 32) && (Copy_digitalValue != NULL))
    {
        // 1. clear MUX 4->0
        ADC_ADMUX &=0b11100000;
        // 2. Select channel
        ADC_ADMUX |= (Copy_channel);
        // 3. Start conversion
        Set_Bit(ADC_ADCSRA, 6);
        // 4. Wait flag=1
        while(Get_Bit(ADC_ADCSRA, 4) == 0);
        // 5. Clear flag
        Set_Bit(ADC_ADCSRA, 4);
        // 6. Read Digital value
        *Copy_digitalValue= ADC_REG;
    }
    else 
    {
        Local_errorCode = StdTypes_OK;
    }

}
