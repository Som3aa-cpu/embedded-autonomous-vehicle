/*
 * GIE_prog.c
 *
 *  Created on: 26 Jul 2026
 *      Author: mohib
 */

 #include "GIE_interface.h"
 #include "INTR_Reg.h"
#include "../../Bit_math.h"
#include "../../StdTypes.h"
void GIE_VoidEnable(void)
{
    Set_Bit(EXIT_uint8_t_SREG,7);
}
void GIE_VoidDisable(void)
{
    Clr_Bit(EXIT_uint8_t_SREG,7);
}
