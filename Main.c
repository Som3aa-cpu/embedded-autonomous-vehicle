/*
 * Main.c
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#include "StdTypes.h"
#include "Bit_math.h"
#include "MCAL/DIO/DIO_interface.h"
#include "MCAL/INTR/INTR_interface.h"
#include "MCAL/INTR/GIE_interface.h"
#include "HAL/HC05/HC05_interface.h"
#include "HAL/IRsensor/IR_interface.h"
#include "HAL/HCSR04/HCSR04_interface.h"
#include "HAL/LCD/LCD_interface.h"
#include "MCAL/I2C/I2C_interface.h"
#include "HAL/LCD/LCD_config.h"
#include "HAL/EEPROM/EEPROM_interface.h"
#include "HAL/EEPROM/EEPROM_priv.h"
#include "HAL/MPU6050/MPU6050_interface.h"
#include "HAL/MPU6050/MPU6050_priv.h"
#include "HAL/MOTOR/Motor_interface.h"
#include "MCAL/TIMERS/Timer_interface.h"
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 11059200UL
#endif
static void Test_vidRunAtSpeed(MotorDirection_t Copy_eLeftDir, MotorDirection_t Copy_eRightDir, uint8_t Copy_u8Speed, uint16_t Copy_u16DurationMs)
{
    uint16_t Local_u16Elapsed = 0U;

    Motor_SetDirection(MOTOR_LEFT, Copy_eLeftDir);
    Motor_SetDirection(MOTOR_RIGHT, Copy_eRightDir);
    Motor_SetSpeed(MOTOR_LEFT, Copy_u8Speed);
    Motor_SetSpeed(MOTOR_RIGHT, Copy_u8Speed);

    while (Local_u16Elapsed < Copy_u16DurationMs)
    {
        _delay_ms(50);
        Local_u16Elapsed += 50U;
    }
}

int main(void)
{
    uint8_t Local_u8Speed;

    Motor_Init();

    while (1)
    {
        /* Step 1: forward */
        Test_vidRunAtSpeed(MOTOR_FORWARD, MOTOR_FORWARD, 50U, 2000U);

        /* Step 2: stop */
        Motor_StopAll();
        _delay_ms(1000);

        /* Step 3: backward */
        Test_vidRunAtSpeed(MOTOR_BACKWARD, MOTOR_BACKWARD, 50U, 2000U);

        /* Step 4: stop */
        Motor_StopAll();
        _delay_ms(1000);

        /* Step 5: turn left in place */
        Test_vidRunAtSpeed(MOTOR_BACKWARD, MOTOR_FORWARD, 50U, 1500U);
        Motor_StopAll();
        _delay_ms(1000);

        /* Step 6: turn right in place */
        Test_vidRunAtSpeed(MOTOR_FORWARD, MOTOR_BACKWARD, 50U, 1500U);
        Motor_StopAll();
        _delay_ms(1000);

        /* Step 8: speed ramp up */
        Motor_SetDirection(MOTOR_LEFT, MOTOR_FORWARD);
        Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
        for (Local_u8Speed = 0U; Local_u8Speed <= 100U; Local_u8Speed += 10U)
        {
            Motor_SetSpeed(MOTOR_LEFT, Local_u8Speed);
            Motor_SetSpeed(MOTOR_RIGHT, Local_u8Speed);
            _delay_ms(300);
        }

        /* ramp down */
        for (Local_u8Speed = 100U; Local_u8Speed > 0U; Local_u8Speed -= 10U)
        {
            Motor_SetSpeed(MOTOR_LEFT, Local_u8Speed);
            Motor_SetSpeed(MOTOR_RIGHT, Local_u8Speed);
            _delay_ms(300);
        }

        /* Step 9: stop, pause before repeating */
        Motor_StopAll();
        _delay_ms(2000);
    }
}
