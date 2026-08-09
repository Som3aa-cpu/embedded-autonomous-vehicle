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
#include "HAL/ESP32/ESP32_interface.h"
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
/* LED on PA5 — blinks when byte received */
#define LED_PORT  DIO_PORTA
#define LED_PIN   DIO_PIN5

int main(void)
{
    USRTInit();
    DIO_setPinDirection(LED_PORT, LED_PIN, DIO_PIN_OUTPUT);
    DIO_setPinValue(LED_PORT, LED_PIN, DIO_PIN_LOW);

    while (1)
    {
        if (USRTDataAvailable())
        {
            uint8_t rx = (uint8_t)USRTReadDataReg();

            if (rx >= 32 && rx <= 127)
            {
                DIO_setPinValue(LED_PORT, LED_PIN, DIO_PIN_HIGH);
                USRTSendDataSync((uint16_t)rx);
            }
            else
            {
                DIO_setPinValue(LED_PORT, LED_PIN, DIO_PIN_LOW);
            }
        }
    }
    return 0;
}
