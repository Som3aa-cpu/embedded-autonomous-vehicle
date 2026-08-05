/*
 * Bit_math.h
 *
 *  Created on: 19 Jul 2026
 *      Author: mohib
 */

#ifndef BIT_MATH_H_
#define BIT_MATH_H_

#define Set_Bit(Var, BitNum)    (Var |= (1 << BitNum))
#define Clr_Bit(Var, BitNum)    (Var &= ~(1 << BitNum))
#define Toggle_Bit(Var, BitNum) (Var ^= (1 << BitNum))
#define Get_Bit(Var, BitNum)    ((Var >> BitNum) & 1)

#endif /* BIT_MATH_H_ */
