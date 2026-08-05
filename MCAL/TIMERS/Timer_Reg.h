/*
 * Timer_Reg.h
 *
 *  Created on: 28 Jul 2026
 *      Author: mohib
 */

#ifndef MCAL_TIMER_REG_H_
#define MCAL_TIMER_REG_H_
 
// Timer 0
#define Timer_TIMSK        *((volatile uint8_t*)0x59)
#define Timer_TIFR          *((volatile uint8_t*)0x58)
#define Timer_TCCR0         *((volatile uint8_t*)0x53)
#define Timer_TCNT0         *((volatile uint8_t*)0x52)
#define Timer_OCR0          *((volatile uint8_t*)0x5C)

// Timer 1
#define Timer_TCCR1A        *((volatile uint8_t*)0x4F)
#define Timer_TCCR1B        *((volatile uint8_t*)0x4E)

#define Timer_TCNT1H        *((volatile uint8_t*)0x4D)
#define Timer_TCNT1L        *((volatile uint8_t*)0x4C)
#define Timer_TCNT1       *((volatile uint16_t*)0x4C)

#define Timer_OCR1AH        *((volatile uint8_t*)0x4B)
#define Timer_OCR1AL        *((volatile uint8_t*)0x4A)
#define Timer_OCR1A         *((volatile uint16_t*)0x4A)

#define Timer_ICR1H           *((volatile uint8_t*)0x47)
#define Timer_ICR1L           *((volatile uint8_t*)0x46)
#define Timer_ICR1            *((volatile uint16_t*)0x46)


// Timer 2
#define Timer_TCCR2         *((volatile uint8_t*)0x45)
#define Timer_TCNT2         *((volatile uint8_t*)0x44)
#define Timer_OCR2          *((volatile uint8_t*)0x43)

// WATCHDOG 
#define WDTCR *((volatile uint8_t*)0x41)




#endif /* MCAL_TIMER_REG_H_ */
