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



/* FIX: forward declaration so EXTI1_voidSetCallback() sees the correct
 * void(*)(void) signature instead of implicitly declaring int HCSR04_voidIcuIsr() */
static void HCSR04_voidIcuIsr(void);

static volatile uint16_t g_u16DistanceCm = 0;
static volatile uint8_t  g_u8EdgeState = 0; /* 0: Waiting Rising, 1: Waiting Falling */
static volatile uint8_t  g_u8NewData = 0;

void HCSR04_voidInit(void)
{
    /* Echo pin MUST be wired to PD3 (INT1 / EXTI1) */
    DIO_setPinDirection(ULTRASONIC_ECHO_PORT, ULTRASONIC_ECHO_PIN, DIO_PIN_INPUT);

    /* FIX: Trigger pin was never configured as an output. ATmega32 pins default
     * to input on reset, so TriggerPulse() was only toggling a pull-up, never
     * actually driving 5V to the sensor's TRIG line. This is why no echo ever came back. */
    DIO_setPinDirection(ULTRASONIC_TRIGGER_PORT, ULTRASONIC_TRIGGER_PIN, DIO_PIN_OUTPUT);
    DIO_setPinValue(ULTRASONIC_TRIGGER_PORT, ULTRASONIC_TRIGGER_PIN, DIO_PIN_LOW);

    EXTI1_voidSetTriggerEdge(EXTI_RISING_EDGE);
    EXTI1_voidSetCallback(HCSR04_voidIcuIsr);
    EXTI1_voidEnable();
}

void HCSR04_voidTriggerPulse(void)
{
    DIO_setPinValue(ULTRASONIC_TRIGGER_PORT, ULTRASONIC_TRIGGER_PIN, DIO_PIN_HIGH);
    /* ~10us delay at 11.0592MHz */
    for (volatile uint8_t i = 0; i < 25; i++);
    DIO_setPinValue(ULTRASONIC_TRIGGER_PORT, ULTRASONIC_TRIGGER_PIN, DIO_PIN_LOW);
    g_u8NewData = 0;
}

uint16_t HCSR04_u16GetDistanceCm(void)
{
    return g_u16DistanceCm;
}

uint8_t HCSR04_u8IsReady(void)
{
    return g_u8NewData;
}

/* ISR called on EXTI1 (PD3 / INT1) */
static void HCSR04_voidIcuIsr(void)
{
    if (g_u8EdgeState == 0)
    {
        /* === RISING EDGE: Start timing === */
        /* CRITICAL FIX: Only reset counter. Do NOT touch TCCR1A/TCCR1B! */
        Timer_TCNT1 = 0;
        /* Next trigger on falling edge */
        EXTI1_voidSetTriggerEdge(EXTI_FALLING_EDGE);
        g_u8EdgeState = 1;
    }
    else
    {
        /* === FALLING EDGE: Read result === */
        uint16_t Local_u16Ticks = Timer_TCNT1;
        /* Calculate distance. Ticks at 11.0592MHz / 8 = 1.3824 MHz */
        /* 1 tick = 0.723 us. Sound travels 0.0343 cm/us. Round trip = divide by 2 */
        /* Distance (cm) = ticks * 0.723 / 58.3 ≈ ticks / 80.6 */
        /* Simplified: ticks / HCSR04_TICKS_PER_CM where HCSR04_TICKS_PER_CM = 58 */
        g_u16DistanceCm = (uint16_t)(Local_u16Ticks / HCSR04_TICKS_PER_CM);
        if (g_u16DistanceCm > 400) g_u16DistanceCm = 400; /* Clamp max */
        g_u8NewData = 1;
        /* Reset for next measurement */
        EXTI1_voidSetTriggerEdge(EXTI_RISING_EDGE);
        g_u8EdgeState = 0;
    }
}
