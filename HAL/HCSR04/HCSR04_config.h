/*
 * HCSR04_config.h
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_HCSR04_HCSR04_CONFIG_H_
#define HAL_HCSR04_HCSR04_CONFIG_H_


#define HCSR04_TICKS_PER_CM 58UL
/* Trigger pin - regular GPIO output */
#define ULTRASONIC_TRIGGER_PORT     DIO_PORTB
#define ULTRASONIC_TRIGGER_PIN      DIO_PIN4

/* Echo pin - fixed to ICP1 (Timer1 Input Capture), not configurable via DIO alone */
#define ULTRASONIC_ECHO_PORT        DIO_PORTD
#define ULTRASONIC_ECHO_PIN         DIO_PIN6


#endif /* HAL_HCSR04_HCSR04_CONFIG_H_ */
