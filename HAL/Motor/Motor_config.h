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

/* PWM pins - both motors now driven by Timer1 (OC1A/OC1B), sharing
 * identical PWM frequency/resolution instead of split across Timer0+Timer1 */
#define MOTOR_LEFT_PWM_PORT     DIO_PORTD
#define MOTOR_LEFT_PWM_PIN      DIO_PIN4   /* OC1B */

#define MOTOR_RIGHT_PWM_PORT    DIO_PORTD
#define MOTOR_RIGHT_PWM_PIN     DIO_PIN5   /* OC1A */

/* L298N direction pins (Left = Channel A, Right = Channel B) */
/* Using PC4-PC7 - PC0/PC1 are reserved for hardware I2C (SCL/SDA, MPU6050) */
#define MOTOR_LEFT_IN1_PORT     DIO_PORTC
#define MOTOR_LEFT_IN1_PIN      DIO_PIN4
#define MOTOR_LEFT_IN2_PORT     DIO_PORTC
#define MOTOR_LEFT_IN2_PIN      DIO_PIN5

#define MOTOR_RIGHT_IN1_PORT    DIO_PORTC
#define MOTOR_RIGHT_IN1_PIN     DIO_PIN6
#define MOTOR_RIGHT_IN2_PORT    DIO_PORTC
#define MOTOR_RIGHT_IN2_PIN     DIO_PIN7

/* Max OCR value - both motors now share Timer1's resolution
 * (Fast PWM mode 14, TOP=ICR1=19999 per Timer1Int()) */
#define MOTOR_PWM_MAX_DUTY       19999u

#endif /* HAL_MOTOR_MOTOR_CONFIG_H_ */
