/*
 * HCSR04.c
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "../../MCAL/INTR/INTR_interface.h"
#include "../../MCAL/TIMERS/Timer_interface.h"
#include "../../MCAL/TIMERS/Timer_Reg.h"
#include "../../MCAL/DIO/DIO_Interface.h"
#include "HCSR04_priv.h"
#include "HCSR04_config.h"
#include "HCSR04_interface.h"


static volatile uint16_t g_u16DistanceCm = 0;
static volatile uint8_t  g_u8EdgeState = 0; /* 0: Waiting for Rising, 1: Waiting for Falling */

void HCSR04_voidInit(void) {
	EXTI1_voidSetTriggerEdge(EXTI_RISING_EDGE);

	EXTI1_voidSetCallback(HCSR04_voidIcuIsr);

	EXTI1_voidEnable();
}

void HCSR04_voidTriggerPulse(void) {
	DIO_setPinValue(DIO_PORTD, DIO_PIN2, DIO_PIN_HIGH);

	for (volatile uint8_t i = 0; i < 20; i++);

	DIO_setPinValue(DIO_PORTD, DIO_PIN2, DIO_PIN_LOW);
}

uint16_t HCSR04_u16GetDistanceCm(void) {
	return g_u16DistanceCm;
}

static void HCSR04_voidIcuIsr(void) {
	if (g_u8EdgeState == 0) {
		/* Rising Edge: Reset & Start Timer1 (Prescaler = 8) */
		Timer_TCNT1 = 0;
		Timer_TCCR1A = 0x00;
		Timer_TCCR1B = (1 << 1);

		/* Configure EXTI1 to trigger next on Falling Edge */
		EXTI1_voidSetTriggerEdge(EXTI_FALLING_EDGE);
		g_u8EdgeState = 1;
	}
	else if (g_u8EdgeState == 1) {
		/* Falling Edge: Read Timer1 count */
		uint16_t Local_u16Ticks = Timer_TCNT1;

		/* Stop Timer1 */
		Timer_TCCR1B = 0x00;

		/* Calculate Distance */
		g_u16DistanceCm = (uint16_t)(Local_u16Ticks / HCSR04_TICKS_PER_CM);

		/* Reset EXTI1 trigger back to Rising Edge */
		EXTI1_voidSetTriggerEdge(EXTI_RISING_EDGE);
		g_u8EdgeState = 0;
	}
}
