/*
 * HCSR04_interface.h
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_HCSR04_HCSR04_INTERFACE_H_
#define HAL_HCSR04_HCSR04_INTERFACE_H_
#include "../../StdTypes.h"

void HCSR04_voidInit(void);
void HCSR04_voidTriggerPulse(void);
uint16_t HCSR04_u16GetDistanceCm(void);
static void HCSR04_voidIcuIsr(void);


#endif /* HAL_HCSR04_HCSR04_INTERFACE_H_ */
