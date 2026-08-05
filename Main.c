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
#include "HAL/HCSR04/HCSR04_interface.h"*
#include <util/delay.h>
int main(void) {
    uint8_t Local_u8RxChar = 0;

    /* 1. Set Pin Directions for UART (PD0 = RX Input, PD1 = TX Output) */
    DIO_setPinDirection(DIO_PORTD, DIO_PIN0, DIO_PIN_INPUT);
    DIO_setPinDirection(DIO_PORTD, DIO_PIN1, DIO_PIN_OUTPUT);

    /* 2. Initialize HC-05 Driver */
    HC05_Init();

    /* 3. Enable Global Interrupts */
    GIE_VoidEnable();

    /* 4. Transmit Initial Banner */
    HC05_SendString((const uint8_t*)"\r\n=================================\r\n");
    HC05_SendString((const uint8_t*)"   HC-05 Standalone Test Ready   \r\n");
    HC05_SendString((const uint8_t*)" Send any character to test echo \r\n");
    HC05_SendString((const uint8_t*)"=================================\r\n");

    while (1) {
        if (HC05_IsDataAvailable() == StdTypes_OK) {
            /* FIX: Changed function name to match HC05_interface.h */
            Local_u8RxChar = HC05_GetReceivedCharAsync();

            HC05_SendString((const uint8_t*)"Received Char: '");
            HC05_SendChar(Local_u8RxChar);
            HC05_SendString((const uint8_t*)"' | ASCII Code: ");
            HC05_SendNumber((uint16_t)Local_u8RxChar);
            HC05_SendString((const uint8_t*)"\r\n");
        }
    }
}
