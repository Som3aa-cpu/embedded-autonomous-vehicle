/*
 * KP_prog.c
 *
 *  Created on: 21 Jul 2026
 *      Author: mohib
 */
#include "KP_interface.h"
#include "../../StdTypes.h"
#include "../../MCAL/DIO/DIO_interface.h"
#include "KP_config.h"

const uint8_t Keypad_Map[KEYPAD_ROWS][KEYPAD_COLS] = {
    { 1,  5,  9, 13 },
    { 2,  6, 10, 14 },
    { 3,  7, 11, 15 },
    { 4,  8, 12, 16 }
};
void KPinit(void)
{
    DIO_setPortDirection(KP_PORT, 0x0F);
    DIO_setPortValue(KP_PORT, 0xFF);
}

uint8_t GetPressed(void)
{
    uint8_t pressed_key = 0;
    uint8_t key_state = DIO_PIN_HIGH;

    for (uint8_t col = 0; col < KEYPAD_COLS; col++)
    {
        DIO_setPinValue(KP_PORT, KP_Col_start + col, DIO_PIN_LOW);

        for (uint8_t row = 0; row < KEYPAD_ROWS; row++)
        {
            DIO_getPinValue(KP_PORT, KP_Row_start + row, &key_state);

            if (key_state == DIO_PIN_LOW)
            {
                pressed_key = Keypad_Map[row][col];


                DIO_setPinValue(KP_PORT, KP_Col_start + col, DIO_PIN_HIGH);
                return pressed_key;
            }
        }

        DIO_setPinValue(KP_PORT, KP_Col_start + col, DIO_PIN_HIGH);
    }

    return pressed_key;
}
