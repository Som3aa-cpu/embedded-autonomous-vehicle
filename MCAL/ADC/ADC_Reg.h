/*
 * ADC_Reg.h
 *
 *  Created on: 27 Jul 2026
 *      Author: mohib
 */

#ifndef ADC_DRIVER_ADC_REG_H_
#define ADC_DRIVER_ADC_REG_H_

#define ADC_ADMUX 	*((volatile uint8_t*)0x27)
#define ADC_ADCSRA 	*((volatile uint8_t*)0x26)
#define ADC_ADCH 	*((volatile uint8_t*)0x25)
#define ADC_ADCL 	*((volatile uint8_t*)0x24)
#define ADC_REG     *((volatile uint16_t*)0x24)



#endif /* ADC_DRIVER_ADC_REG_H_ */
