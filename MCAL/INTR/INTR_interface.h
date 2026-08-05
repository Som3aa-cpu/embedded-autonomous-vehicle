/*
 * INTR_interface.h
 *
 *  Created on: 26 Jul 2026
 *      Author: mohib
 */

#ifndef MCAL_INTR_INTERFACE_H_
#define MCAL_INTR_INTERFACE_H_
#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "GIE_interface.h"


#define EXTI_LOW_LEVEL     0
#define EXTI_ON_CHANGE     1
#define EXTI_FALLING_EDGE  2
#define EXTI_RISING_EDGE   3

void Exit_voidExti0enable(void);
void EXTI1_voidDisable(void);
void EXTI1_voidSetTriggerEdge(uint8_t Copy_u8Edge);
void EXTI1_voidSetCallback(PF LPF);



#endif /* MCAL_INTR_INTERFACE_H_ */
