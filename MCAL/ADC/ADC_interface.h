/*
 * ADC_interface.h
 *
 *  Created on: 27 Jul 2026
 *      Author: mohib
 */

#ifndef ADC_DRIVER_ADC_INTERFACE_H_
#define ADC_DRIVER_ADC_INTERFACE_H_
#include "../../StdTypes.h"

#define ADC_Channel_0   0         
#define ADC_Channel_1   1         
#define ADC_Channel_2   2
#define ADC_Channel_3   3
#define ADC_Channel_4   4
#define ADC_Channel_5   5
#define ADC_Channel_6   6
#define ADC_Channel_7   7


void ADC_init(void);

uint8_t ADC_GetDigitalValue(uint8_t Copy_channel, uint16_t *Copy_digitalValue);






#endif /* ADC_DRIVER_ADC_INTERFACE_H_ */
