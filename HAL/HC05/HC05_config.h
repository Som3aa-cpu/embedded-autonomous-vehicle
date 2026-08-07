/*
 * HC05_config.h
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_HC05_HC05_CONFIG_H_
#define HAL_HC05_HC05_CONFIG_H_

#define BLUETOOTH_RXD_PORT     DIO_PORTD
#define BLUETOOTH_RXD_PIN      DIO_PIN0   /* HC-05 TXD -> ATmega RXD */

#define BLUETOOTH_TXD_PORT     DIO_PORTD
#define BLUETOOTH_TXD_PIN      DIO_PIN1   /* ATmega TXD -> HC-05 RXD */

/* UART baud rate for HC-05 communication (default module baud) */
#define BLUETOOTH_BAUD_RATE    9600UL
#define HC05_MAX_BUFFER_SIZE  64


#endif /* HAL_HC05_HC05_CONFIG_H_ */
