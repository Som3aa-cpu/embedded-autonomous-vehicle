/*
 * INTR_prog.c
 *
 *  Created on: 26 Jul 2026
 *      Author: mohib
 */

#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "../DIO/DIO_interface.h"
#include "INTR_interface.h"
#include "INTR_Reg.h"

static PF EXTI1_GPF = NULL;

void EXTI1_voidEnable(void)
{
    /* Enable EXTI1 (Bit 7 in GICR) */
    Set_Bit(EXIT_uint8_t_GICR, 7);
}

void EXTI1_voidDisable(void)
{
    /* Disable EXTI1 */
    Clr_Bit(EXIT_uint8_t_GICR, 7);
}

void EXTI1_voidSetTriggerEdge(uint8_t Copy_u8Edge)
{
    if (Copy_u8Edge == EXTI_RISING_EDGE)
    {
        Set_Bit(EXIT_uint8_t_MCUCR, 2);
        Set_Bit(EXIT_uint8_t_MCUCR, 3);
    }
    else if (Copy_u8Edge == EXTI_FALLING_EDGE)
    {
        Clr_Bit(EXIT_uint8_t_MCUCR, 2);
        Set_Bit(EXIT_uint8_t_MCUCR, 3);
    }
}

void EXTI1_voidSetCallback(PF LPF)
{
    if (LPF != NULL)
    {
        EXTI1_GPF = LPF;
    }
}

/* INT1 ISR Vector */
void __vector_2(void) __attribute__((signal));
void __vector_2(void)
{
    if (EXTI1_GPF != NULL)
    {
        EXTI1_GPF();
    }
}
