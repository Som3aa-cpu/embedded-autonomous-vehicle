/*
 * HCSR04_config.h
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_HCSR04_HCSR04_CONFIG_H_
#define HAL_HCSR04_HCSR04_CONFIG_H_


#define HCSR04_TICKS_PER_CM         58UL

/* Trigger pin — PB4, regular GPIO output */
#define ULTRASONIC_TRIGGER_PORT     DIO_PORTB
#define ULTRASONIC_TRIGGER_PIN      DIO_PIN4

/* Echo pin — PD3 (INT1 / EXTI1), NOT PD6/ICP1 */
/* The driver uses EXTI1 interrupt, not Timer1 Input Capture */
#define ULTRASONIC_ECHO_PORT        DIO_PORTD
#define ULTRASONIC_ECHO_PIN         DIO_PIN3


#endif /* HAL_HCSR04_HCSR04_CONFIG_H_ */
