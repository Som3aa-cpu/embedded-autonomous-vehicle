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

void EXTI0_voidEnable(void)
{
    //1 select edge source
    Set_Bit(EXIT_uint8_t_MCUCR,0);
    //2 clear interrupt flag
    Clr_Bit(EXIT_uint8_t_MCUCR,1);

    // enable EXIT0
    Set_Bit(EXIT_uint8_t_GICR,6);
}
void STUB()
{

}
PF GPF=&STUB;
void EXIT_VoidSetCallback(PF LPF)
{
    GPF = LPF;
}
