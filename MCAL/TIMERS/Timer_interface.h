/*
 * Timer_interface.h
 *
 *  Created on: 28 Jul 2026
 *      Author: mohib
 */

#ifndef MCAL_TIMER_INTERFACE_H_
#define MCAL_TIMER_INTERFACE_H_

#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "Timer_Reg.h"

// TIMER 0 & 1
void Timer0Int(void);
void FastPWNinit(void);
void PhasePWMInit(void);
void Timer1Int(void);
void Timer0CompareMatchValue(uint8_t OCR0value);

uint8_t TimerSetCallBack(PF LPF);

uint8_t TimerSetCTCCallBack(PF LPF);
void Timer1CompareMatchValue(uint16_t OCR1value);

// TIMER 2
void Timer2_Init(void);

void Timer2_EnableOvfInterrupt(void);
void Timer2_DisableOvfInterrupt(void);

void Timer2_EnableCompareInterrupt(void);
void Timer2_DisableCompareInterrupt(void);

void Timer2_SetCallBackOvf(void (*pLocalFunction)(void));
void Timer2_SetCallBackCompare(void (*pLocalFunction)(void));

void Timer2_SetTCNTValue(uint8_t Value);
void Timer2_SetOCRValue(uint8_t Value);

uint8_t Timer2_ReadTCNTValue(void);
void Timer2_SetClockSelect(uint8_t Prescaler);

// WATCH DOG TIMER
void WDT_Enable(void);
void WDT_Disable(void);

// ICU
#define ICU_FALLING_EDGE    0
#define ICU_RISING_EDGE     1

void ICU_Init(uint8_t copy_Edge);
void ICU_SetTriggerEdge(uint8_t copy_Edge);
void ICU_EnableInterrupt(void);
void ICU_DisableInterrupt(void);
uint16_t ICU_GetCapturedValue(void);
void ICU_ClearTimer(void);
void ICU_SetCallBack(PF LFP);






#endif /* MCAL_TIMER_INTERFACE_H_ */
