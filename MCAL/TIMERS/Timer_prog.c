/*
 * Timer_prog.c
 *
 *  Created on: 28 Jul 2026
 *      Author: mohib
 */
#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "Timer_interface.h"
#include "Timer_Reg.h"

static void (*Timer2_pCallBackOvf)(void) = NULL;
static void (*Timer2_pCallBackCompare)(void) = NULL;
static void(*TimerPftmerOVF)(void) = NULL;
static void(*TimerPftmerCTC)(void) = NULL;
static void (*g_ICU_CallBackPtr)(void) = NULL;

// TIMER 0 & 1

void Timer0Int(void)
{
	// 1. select normal
	Clr_Bit(Timer_TCCR0 ,6);
	Clr_Bit(Timer_TCCR0 ,3);
	// 2. enable ovf Intruppet
	Set_Bit(Timer_TIMSK ,0);
	// 3. set preload
	Timer_TCNT0 = 192;
	// 4. set prescaler
	Clr_Bit(Timer_TCCR0 ,0);
	Set_Bit(Timer_TCCR0 ,1);
	Clr_Bit(Timer_TCCR0 ,2);
}

// select mode -> Fast PWN mode
void FastPWNinit()
{
	// Mode: Fast PWM
	Set_Bit(Timer_TCCR0, 6);
	Set_Bit(Timer_TCCR0, 3);

	// Non-inverting Mode (COM01 = 1, COM00 = 0)
	Set_Bit(Timer_TCCR0, 5);
	Clr_Bit(Timer_TCCR0, 4);

	// Prescaler = 8
	Clr_Bit(Timer_TCCR0, 0);
	Set_Bit(Timer_TCCR0, 1);
	Clr_Bit(Timer_TCCR0, 2);
}

void PhasePWMInit(void)
{
	Set_Bit(Timer_TCCR0, 6); // WGM00 = 1
	Clr_Bit(Timer_TCCR0, 3); // WGM01 = 0

	Set_Bit(Timer_TCCR0, 5); // COM01 = 1
	Clr_Bit(Timer_TCCR0, 4); // COM00 = 0

	Timer_OCR0 = 127;

	Clr_Bit(Timer_TCCR0, 0); // CS00 = 0
	Set_Bit(Timer_TCCR0, 1); // CS01 = 1
	Clr_Bit(Timer_TCCR0, 2); // CS02 = 0
}

void Timer1Int(void)
{
	// timer 1 mode 14
	Clr_Bit(Timer_TCCR1A, 0);
	Set_Bit(Timer_TCCR1A, 1);

	Set_Bit(Timer_TCCR1B, 3);
	Set_Bit(Timer_TCCR1B, 4);

	// select hardware action None inverting
	Clr_Bit(Timer_TCCR1A, 6);
	Set_Bit(Timer_TCCR1A, 7);

	// Top value in ICRA
	Timer_ICR1 = 19999;

	Clr_Bit(Timer_TCCR1B, 0);
	Set_Bit(Timer_TCCR1B, 1);
	Clr_Bit(Timer_TCCR1B, 2);
}

void Timer0CompareMatchValue(uint8_t OCR0value)
{
	Timer_OCR0 = OCR0value;
}
void Timer1CompareMatchValue(uint16_t OCR1value)
{
	Timer_OCR1A = OCR1value;
}


uint8_t TimerSetCallBack(PF LPF)
{
	if (LPF != NULL)
	{
		TimerPftmerOVF = LPF;
		return StdTypes_OK;
	}
	return StdTypes_NOK;
}

uint8_t TimerSetCTCCallBack(PF LPF)
{
	if (LPF != NULL)
	{
		TimerPftmerCTC = LPF;
		return StdTypes_OK;
	}
	return StdTypes_NOK;
}

//--------------------------------------------------------------------------------------------
// TIMER 2

void Timer2_Init(void)
{
	// 1. select normal mode (WGM20 = 0, WGM21 = 0)
	Clr_Bit(Timer_TCCR2, 6);
	Clr_Bit(Timer_TCCR2, 3);

	// 2. enable ovf Interrupt (TOIE2 is bit 6 in TIMSK)
	Set_Bit(Timer_TIMSK, 6);

	// 3. set preload
	Timer_TCNT2 = 192;

	// 4. set prescaler = 8 (CS22:20 = 010 -> CS20=0, CS21=1, CS22=0)
	Clr_Bit(Timer_TCCR2, 0);
	Set_Bit(Timer_TCCR2, 1);
	Clr_Bit(Timer_TCCR2, 2);
}
void Timer2_EnableOvfInterrupt(void)
{
	Set_Bit(Timer_TIMSK, 6);
}
void Timer2_DisableOvfInterrupt(void)
{
	Clr_Bit(Timer_TIMSK, 6);
}
void Timer2_EnableCompareInterrupt(void)
{
	Set_Bit(Timer_TIMSK, 7);
}
void Timer2_DisableCompareInterrupt(void)
{
	Clr_Bit(Timer_TIMSK, 7);
}

void Timer2_SetCallBackOvf(void (*pLocalFunction)(void))
{
	Timer2_pCallBackOvf = pLocalFunction;
}
void Timer2_SetCallBackCompare(void (*pLocalFunction)(void))
{
	Timer2_pCallBackCompare = pLocalFunction;
}
void Timer2_SetTCNTValue(uint8_t Value)
{
	Timer_TCNT2 = Value;
}
void Timer2_SetOCRValue(uint8_t Value)
{
	Timer_OCR2 = Value;
}
uint8_t Timer2_ReadTCNTValue(void)
{
	return Timer_TCNT2;
}

void __vector_11(void)  __attribute__((signal));
void __vector_11(void)
{
	static uint16_t Local_counterOVF=0;
	Timer_TCNT0 = 192;
	Local_counterOVF++;
	if(Local_counterOVF == 3907)
	{
		if(TimerPftmerOVF != NULL)
		{
			TimerPftmerOVF();
		}
		Local_counterOVF = 0;
	}
}

// -------------------------------------------------------------------------------------------------------------------
// WATCH DOG TIMER
void WDT_Enable(void)
{
	WDTCR= 0b00001111;
}
void WDT_Disable(void)
{
	WDTCR= 0b00011000;
	WDTCR= 0x00;
}

//----------------------------------------------------------------------------------------
// ICU

void ICU_Init(uint8_t copy_Edge)
{
	Clr_Bit(Timer_TCCR1A, 0);
	Clr_Bit(Timer_TCCR1A, 1);
	Clr_Bit(Timer_TCCR1B, 3);
	Clr_Bit(Timer_TCCR1B, 4);

	Clr_Bit(Timer_TCCR1B, 0);
	Set_Bit(Timer_TCCR1B, 1);
	Clr_Bit(Timer_TCCR1B, 2);

	if (copy_Edge == ICU_RISING_EDGE)
	{
		Set_Bit(Timer_TCCR1B, 6);
	}
	else
	{
		Clr_Bit(Timer_TCCR1B, 6);
	}
}
void ICU_SetTriggerEdge(uint8_t copy_Edge)
{
	if (copy_Edge == ICU_RISING_EDGE)
	{
		Set_Bit(Timer_TCCR1B, 6);
	}
	else
	{
		Clr_Bit(Timer_TCCR1B, 6);
	}
}
void ICU_EnableInterrupt(void)
{
	Set_Bit(Timer_TIMSK, 5);
}
void ICU_DisableInterrupt(void)
{
	Clr_Bit(Timer_TIMSK, 5);
}
uint16_t ICU_GetCapturedValue(void)
{
	return Timer_ICR1;
}
void ICU_ClearTimer(void)
{
	Timer_TCNT1 = 0;
}
void ICU_SetCallBack(PF LPF)
{
	if (LPF != NULL)
	{
		g_ICU_CallBackPtr = LPF;
	}
}

//----------------------------------------------------------------------------------------------------
// HARDWARE PROTOCOLS
void __vector_10(void)  __attribute__((signal));
void __vector_10(void)
{
	static uint16_t Local_counterCTC=0;
	Local_counterCTC++;
	if(Local_counterCTC == 3907)
	{
		if(TimerPftmerCTC != NULL)
		{
			TimerPftmerCTC();
		}
	}
}
void __vector_4(void) __attribute__((signal));
void __vector_4(void)
{
	if (Timer2_pCallBackCompare != NULL)
	{
		Timer2_pCallBackCompare();
	}
}

void __vector_5(void) __attribute__((signal));
void __vector_5(void)
{
	Timer_TCNT2 = 192;
	if (Timer2_pCallBackOvf != NULL)
	{
		Timer2_pCallBackOvf();
	}
}
void __vector_6(void) __attribute__((signal));
void __vector_6(void)
{
	if (g_ICU_CallBackPtr != NULL)
	{
		g_ICU_CallBackPtr();
	}
}




