/*
 * INTR_Reg.h
 *
 *  Created on: 26 Jul 2026
 *      Author: mohib
 */

#ifndef MCAL_INTR_REG_H_
#define MCAL_INTR_REG_H_

#define EXIT_uint8_t_MCUCR          *((volatile uint8_t*)0x55)
#define EXIT_uint8_t_MCUCSR          *((volatile uint8_t*)0x54)

#define EXIT_uint8_t_GICR         *((volatile uint8_t*)0x5B)
#define EXIT_uint8_t_GIFR         *((volatile uint8_t*)0x5A)

#define EXIT_uint8_t_SREG         *((volatile uint8_t*)0x5F)

#endif /* MCAL_INTR_REG_H_ */
