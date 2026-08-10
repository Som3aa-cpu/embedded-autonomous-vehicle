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

/* ==========================================================================
 *  TIMER 0
 * ========================================================================== */
void Timer0Int(void)
{
    Clr_Bit(Timer_TCCR0, 6);
    Clr_Bit(Timer_TCCR0, 3);
    Set_Bit(Timer_TIMSK, 0);
    Timer_TCNT0 = 192;
    Clr_Bit(Timer_TCCR0, 0);
    Set_Bit(Timer_TCCR0, 1);
    Clr_Bit(Timer_TCCR0, 2);
}

void FastPWNinit(void)
{
    Set_Bit(Timer_TCCR0, 6);
    Set_Bit(Timer_TCCR0, 3);
    Set_Bit(Timer_TCCR0, 5);
    Clr_Bit(Timer_TCCR0, 4);
    Clr_Bit(Timer_TCCR0, 0);
    Set_Bit(Timer_TCCR0, 1);
    Clr_Bit(Timer_TCCR0, 2);
}

void PhasePWMInit(void)
{
    Set_Bit(Timer_TCCR0, 6);
    Clr_Bit(Timer_TCCR0, 3);
    Set_Bit(Timer_TCCR0, 5);
    Clr_Bit(Timer_TCCR0, 4);
    Timer_OCR0 = 127;
    Clr_Bit(Timer_TCCR0, 0);
    Set_Bit(Timer_TCCR0, 1);
    Clr_Bit(Timer_TCCR0, 2);
}

/* ==========================================================================
 *  TIMER 1 — Fast PWM Mode 14, ICR1 = TOP = 19999
 *  OC1A (PD5) = Right motor, OC1B (PD4) = Left motor
 * ========================================================================== */
void Timer1Int(void)
{
    /* WGM13:12:11:10 = 1:1:1:0 → Mode 14 (Fast PWM, ICR1 = TOP) */
    Clr_Bit(Timer_TCCR1A, 0);  /* WGM10 = 0 */
    Set_Bit(Timer_TCCR1A, 1);  /* WGM11 = 1 */
    Set_Bit(Timer_TCCR1B, 3);  /* WGM12 = 1 */
    Set_Bit(Timer_TCCR1B, 4);  /* WGM13 = 1 */

    /* COM1A1:0 = 10 → Non-inverting PWM on OC1A (PD5, Right motor) */
    Clr_Bit(Timer_TCCR1A, 6);
    Set_Bit(Timer_TCCR1A, 7);

    /* COM1B1:0 = 10 → Non-inverting PWM on OC1B (PD4, Left motor) */
    Clr_Bit(Timer_TCCR1A, 4);  /* COM1B0 = 0 */
    Set_Bit(Timer_TCCR1A, 5);  /* COM1B1 = 1 ← THIS WAS MISSING */

    /* TOP value for ~69Hz @ 11.0592MHz, prescaler 8 */
    Timer_ICR1 = 19999;

    /* Prescaler = 8 (CS12:11:10 = 010) */
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

void Timer1B_CompareMatchValue(uint16_t OCR1Bvalue)
{
    Timer_OCR1B = OCR1Bvalue;
}

/* ==========================================================================
 *  CALLBACKS
 * ========================================================================== */
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

/* ==========================================================================
 *  TIMER 2
 * ========================================================================== */
void Timer2_Init(void)
{
    Clr_Bit(Timer_TCCR2, 6);
    Clr_Bit(Timer_TCCR2, 3);
    Set_Bit(Timer_TIMSK, 6);
    Timer_TCNT2 = 192;
    Clr_Bit(Timer_TCCR2, 0);
    Set_Bit(Timer_TCCR2, 1);
    Clr_Bit(Timer_TCCR2, 2);
}

void Timer2_EnableOvfInterrupt(void)   { Set_Bit(Timer_TIMSK, 6); }
void Timer2_DisableOvfInterrupt(void)  { Clr_Bit(Timer_TIMSK, 6); }
void Timer2_EnableCompareInterrupt(void) { Set_Bit(Timer_TIMSK, 7); }
void Timer2_DisableCompareInterrupt(void){ Clr_Bit(Timer_TIMSK, 7); }

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

/* ==========================================================================
 *  WATCHDOG
 * ========================================================================== */
void WDT_Enable(void)
{
    WDTCR = 0b00001111;
}

void WDT_Disable(void)
{
    WDTCR = 0b00011000;
    WDTCR = 0x00;
}

/* ==========================================================================
 *  ICU — Input Capture Unit on ICP1 (PD6)
 *  CRITICAL FIX: Does NOT touch WGM bits! Preserves Fast PWM Mode 14.
 * ========================================================================== */
void ICU_Init(uint8_t copy_Edge)
{
    /* Do NOT touch WGM bits — Timer1 is already in Fast PWM Mode 14 */
    /* Only set edge trigger and ensure prescaler is 8 */

    /* Set prescaler = 8 (same as Timer1 PWM, must match) */
    Clr_Bit(Timer_TCCR1B, 0);
    Set_Bit(Timer_TCCR1B, 1);
    Clr_Bit(Timer_TCCR1B, 2);

    /* Set initial trigger edge */
    if (copy_Edge == ICU_RISING_EDGE)
    {
        Set_Bit(Timer_TCCR1B, 6);  /* ICES1 = 1 */
    }
    else
    {
        Clr_Bit(Timer_TCCR1B, 6);  /* ICES1 = 0 */
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
    Set_Bit(Timer_TIMSK, 5);  /* TICIE1 */
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

/* ==========================================================================
 *  ISRs
 * ========================================================================== */
void __vector_11(void) __attribute__((signal));
void __vector_11(void)
{
    static uint16_t Local_counterOVF = 0;
    Timer_TCNT0 = 192;
    Local_counterOVF++;
    if (Local_counterOVF == 3907)
    {
        if (TimerPftmerOVF != NULL)
        {
            TimerPftmerOVF();
        }
        Local_counterOVF = 0;
    }
}

void __vector_10(void) __attribute__((signal));
void __vector_10(void)
{
    static uint16_t Local_counterCTC = 0;
    Local_counterCTC++;
    if (Local_counterCTC == 3907)
    {
        if (TimerPftmerCTC != NULL)
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
