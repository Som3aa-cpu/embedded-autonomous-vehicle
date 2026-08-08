/*
 * Motor.c
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#include "Motor_interface.h"
#include "Motor_config.h"
#include "../../MCAL/TIMERS/Timer_interface.h"
#include "../../MCAL/DIO/DIO_Interface.h"

static uint16_t PercentToDuty(uint8_t speedPercent)
{
    if (speedPercent > 100u)
    {
        speedPercent = 100u;
    }
    return (uint16_t)(((uint32_t)speedPercent * MOTOR_PWM_MAX_DUTY) / 100u);
}

void Motor_Init(void)
{
    /* PWM output pins */
    DIO_setPinDirection(MOTOR_LEFT_PWM_PORT, MOTOR_LEFT_PWM_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(MOTOR_RIGHT_PWM_PORT, MOTOR_RIGHT_PWM_PIN, DIO_PIN_OUTPUT);

    /* Direction pins */
    DIO_setPinDirection(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN, DIO_PIN_OUTPUT);

    /* Configures Timer1 (OC1A + OC1B) in Fast PWM mode 14, non-inverting -
     * drives both motors from the same timer/frequency */
    Timer1Int();

    Timer1B_CompareMatchValue(0);
    Timer1CompareMatchValue(0);

    Motor_StopAll();
}

void Motor_SetDirection(Motor_t motor, MotorDirection_t direction)
{
    uint8_t in1Port, in1Pin, in2Port, in2Pin;

    if (motor == MOTOR_LEFT)
    {
        in1Port = MOTOR_LEFT_IN1_PORT;
        in1Pin  = MOTOR_LEFT_IN1_PIN;
        in2Port = MOTOR_LEFT_IN2_PORT;
        in2Pin  = MOTOR_LEFT_IN2_PIN;
    }
    else
    {
        in1Port = MOTOR_RIGHT_IN1_PORT;
        in1Pin  = MOTOR_RIGHT_IN1_PIN;
        in2Port = MOTOR_RIGHT_IN2_PORT;
        in2Pin  = MOTOR_RIGHT_IN2_PIN;
    }

    switch (direction)
    {
        case MOTOR_FORWARD:
            DIO_setPinValue(in1Port, in1Pin, DIO_PIN_LOW);
            DIO_setPinValue(in2Port, in2Pin, DIO_PIN_HIGH);
            break;

        case MOTOR_BACKWARD:
            DIO_setPinValue(in1Port, in1Pin, DIO_PIN_HIGH);
            DIO_setPinValue(in2Port, in2Pin, DIO_PIN_LOW);
            break;

        case MOTOR_BRAKE:
        default:
            DIO_setPinValue(in1Port, in1Pin, DIO_PIN_HIGH);
            DIO_setPinValue(in2Port, in2Pin, DIO_PIN_HIGH);
            break;
    }
}

void Motor_SetSpeed(Motor_t motor, uint8_t speedPercent)
{
    uint16_t duty = PercentToDuty(speedPercent);

    if (motor == MOTOR_LEFT)
    {
        Timer1B_CompareMatchValue(duty);
    }
    else
    {
        Timer1CompareMatchValue(duty);
    }
}

void Motor_Stop(Motor_t motor)
{
    Motor_SetSpeed(motor, 0u);
}

void Motor_StopAll(void)
{
    Motor_Stop(MOTOR_LEFT);
    Motor_Stop(MOTOR_RIGHT);
}
