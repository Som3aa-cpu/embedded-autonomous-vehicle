/*
 * Motor_interface.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_MOTOR_MOTOR_INTERFACE_H_
#define HAL_MOTOR_MOTOR_INTERFACE_H_

#include "../../StdTypes.h"
#include "../../Bit_math.h"

typedef enum
{
    MOTOR_LEFT = 0,
    MOTOR_RIGHT
} Motor_t;

typedef enum
{
    MOTOR_FORWARD = 0,
    MOTOR_BACKWARD,
    MOTOR_BRAKE
} MotorDirection_t;

/* Initializes PWM timers and direction pins, motors start stopped */
void Motor_Init(void);

/* Sets rotation direction of a single motor group */
void Motor_SetDirection(Motor_t motor, MotorDirection_t direction);

/* Sets speed of a single motor group, speedPercent range: 0-100 */
void Motor_SetSpeed(Motor_t motor, uint8_t speedPercent);

/* Stops a single motor group (speed = 0, direction unchanged) */
void Motor_Stop(Motor_t motor);

/* Stops both motor groups */
void Motor_StopAll(void);



#endif /* HAL_MOTOR_MOTOR_INTERFACE_H_ */
