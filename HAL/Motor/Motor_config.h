/*
 * Motor_config.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_MOTOR_MOTOR_CONFIG_H_
#define HAL_MOTOR_MOTOR_CONFIG_H_


#include "../../StdTypes.h"
#include "../../Bit_math.h"

/* PWM pins - driven by Timer hardware (must still be set as DIO output) */
#define MOTOR_LEFT_PWM_PORT     DIO_PORTB
#define MOTOR_LEFT_PWM_PIN      DIO_PIN3   /* OC0  */

#define MOTOR_RIGHT_PWM_PORT    DIO_PORTD
#define MOTOR_RIGHT_PWM_PIN     DIO_PIN5   /* OC1A */

/* L298N direction pins (Left = Channel A, Right = Channel B) */
/* Using PORTC - PORTA is reserved for the IR sensor array */
#define MOTOR_LEFT_IN1_PORT     DIO_PORTC
#define MOTOR_LEFT_IN1_PIN      DIO_PIN0
#define MOTOR_LEFT_IN2_PORT     DIO_PORTC
#define MOTOR_LEFT_IN2_PIN      DIO_PIN1

#define MOTOR_RIGHT_IN1_PORT    DIO_PORTC
#define MOTOR_RIGHT_IN1_PIN     DIO_PIN2
#define MOTOR_RIGHT_IN2_PORT    DIO_PORTC
#define MOTOR_RIGHT_IN2_PIN     DIO_PIN3

/* Max OCR value for the PWM resolution both timers are configured for (8-bit) */
#define MOTOR_PWM_MAX_DUTY      255u

#endif /* HAL_MOTOR_MOTOR_CONFIG_H_ */
