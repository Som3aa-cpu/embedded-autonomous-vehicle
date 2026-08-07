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
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 11059200UL
#endif
#define LED_PORT    DIO_PORTB
#define LED_PIN     DIO_PIN0

int main(void)
{
    MPU6050_RawData_t Local_stRaw;
    float Local_f32Ax, Local_f32Ay, Local_f32Az;
    float Local_f32Gx, Local_f32Gy, Local_f32Gz;
    char  Local_acLine[22];

    DIO_setPinDirection(LED_PORT, LED_PIN, DIO_PIN_OUTPUT);
    DIO_setPinValue(LED_PORT, LED_PIN, DIO_PIN_LOW);

    DIO_setPinDirection(DIO_PORTC, DIO_PIN0, DIO_PIN_INPUT);
    DIO_setPinDirection(DIO_PORTC, DIO_PIN1, DIO_PIN_INPUT);
    DIO_activePullUpRessistencePin(DIO_PORTC, DIO_PIN0);
    DIO_activePullUpRessistencePin(DIO_PORTC, DIO_PIN1);

    SSD1306_vidInit();
    MPU6050_vidInit();

    /* Connection check first - if this fails, nothing else will work */
    if (MPU6050_u8CheckConnection() != MPU6050_OK)
    {
        SSD1306_vidClearBuffer();
        SSD1306_vidWriteString(0, 0, "MPU6050 NOT FOUND");
        SSD1306_vidWriteString(0, 2, "Check wiring/addr");
        SSD1306_u8UpdateScreen();

        while (1)
        {
            DIO_togglePinValues(LED_PORT, LED_PIN);
            _delay_ms(150);
        }
    }

    /* Connected - LED solid ON as a quick at-a-glance confirmation */
    DIO_setPinValue(LED_PORT, LED_PIN, DIO_PIN_HIGH);

    while (1)
    {
        if (MPU6050_u8ReadRawData(&Local_stRaw) == MPU6050_OK)
        {
            MPU6050_vidGetAccel_g(&Local_stRaw, &Local_f32Ax, &Local_f32Ay, &Local_f32Az);
            MPU6050_vidGetGyro_dps(&Local_stRaw, &Local_f32Gx, &Local_f32Gy, &Local_f32Gz);

            SSD1306_vidClearBuffer();
            SSD1306_vidWriteString(0, 0, "MPU6050 LIVE DATA");

            /* avr-libc's sprintf doesn't support %f by default (needs an extra
             * linked library), so values are shown as g x100 / dps x100 -
             * e.g. "105" means 1.05g, "-32" means -0.32 dps */
            sprintf(Local_acLine, "Ax:%d Ay:%d Az:%d", (int)(Local_f32Ax * 100), (int)(Local_f32Ay * 100), (int)(Local_f32Az * 100));
            SSD1306_vidWriteString(0, 2, Local_acLine);

            sprintf(Local_acLine, "Gx:%d Gy:%d Gz:%d", (int)(Local_f32Gx * 100), (int)(Local_f32Gy * 100), (int)(Local_f32Gz * 100));
            SSD1306_vidWriteString(0, 4, Local_acLine);

            SSD1306_u8UpdateScreen();
        }

        _delay_ms(200);
    }
}
