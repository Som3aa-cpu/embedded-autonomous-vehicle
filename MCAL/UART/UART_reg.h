/*
 * UART_reg.h
 *
 *  Created on: 2 Aug 2026
 *      Author: mohib
 */

#ifndef MCAL_UART_UART_REG_H_
#define MCAL_UART_UART_REG_H_

#include "../../StdTypes.h"

#define UDR     (*((volatile uint8_t *)0x2C))
#define UCSRA   (*((volatile uint8_t *)0x2B))
#define UCSRB   (*((volatile uint8_t *)0x2A))
#define UBRRL   (*((volatile uint8_t *)0x29))

/* UCSRC and UBRRH Share Address 0x40 (Selected via URSEL bit) */
#define UCSRC   (*((volatile uint8_t *)0x40))
#define UBRRH   (*((volatile uint8_t *)0x40))

#endif /* MCAL_UART_UART_REG_H_ */
