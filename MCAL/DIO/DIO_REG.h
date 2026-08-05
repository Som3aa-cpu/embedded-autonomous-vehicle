/*
 * DIO_REG.h
 *
 *  Created on: 19 Jul 2026
 *      Author: mohib
 */

#ifndef MCAL_DIO_REG_H_
#define MCAL_DIO_REG_H_
// PORT A
#define PORTA_Register *((volatile uint8_t*)0x3B)
#define DDRA_Register *((volatile uint8_t*)0x3A)
#define PINA_Register *((volatile uint8_t*)0x39)

// PORT B
#define PORTB_Register *((volatile uint8_t*)0x38)
#define DDRB_Register *((volatile uint8_t*)0x37)
#define PINB_Register *((volatile uint8_t*)0x36)

//PORT C
#define PORTC_Register *((volatile uint8_t*)0x35)
#define DDRC_Register *((volatile uint8_t*)0x34)
#define PINC_Register *((volatile uint8_t*)0x33)

// PORT D
#define PORTD_Register *((volatile uint8_t*)0x32)
#define DDRD_Register *((volatile uint8_t*)0x31)
#define PIND_Register *((volatile uint8_t*)0x30) 
#endif /* MCAL_DIO_REG_H_ */
