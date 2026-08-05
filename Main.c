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
#include "HAL/HCSR04/HCSR04_interface.h"*
#include <util/delay.h>
int main(void) {
    uint8_t local_u8RawValues = 0;

    /* Initialize IR driver (Inputs on PORTA) */
    IR_voidInit();

    /* Set PORTB0 - PORTB4 as OUTPUTS for test LEDs */
    DIO_setPinDirection(DIO_PORTB, DIO_PIN0, DIO_PIN_OUTPUT);
    DIO_setPinDirection(DIO_PORTB, DIO_PIN1, DIO_PIN_OUTPUT);
    DIO_setPinDirection(DIO_PORTB, DIO_PIN2, DIO_PIN_OUTPUT);
    DIO_setPinDirection(DIO_PORTB, DIO_PIN3, DIO_PIN_OUTPUT);
    DIO_setPinDirection(DIO_PORTB, DIO_PIN4, DIO_PIN_OUTPUT);

    while (1) {
        /* Read raw 5-bit packed sensor values */
        local_u8RawValues = IR_u8ReadRaw();

        /* Mirror IR sensor digital states directly to PORTB LEDs */
        DIO_setPinValue(DIO_PORTB, DIO_PIN0, Get_Bit(local_u8RawValues, 0));
        DIO_setPinValue(DIO_PORTB, DIO_PIN1, Get_Bit(local_u8RawValues, 1));
        DIO_setPinValue(DIO_PORTB, DIO_PIN2, Get_Bit(local_u8RawValues, 2));
        DIO_setPinValue(DIO_PORTB, DIO_PIN3, Get_Bit(local_u8RawValues, 3));
        DIO_setPinValue(DIO_PORTB, DIO_PIN4, Get_Bit(local_u8RawValues, 4));

        _delay_ms(50);
    }

    return 0;
}
