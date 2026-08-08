/*
 * UART_config.h
 *
 *  Created on: 2 Aug 2026
 *      Author: mohib
 */
#ifndef MCAL_UART_UART_CONFIG_H_
#define MCAL_UART_UART_CONFIG_H_

#define USART_DOUBLE_SPEED_DISABLE  0
#define USART_DOUBLE_SPEED_ENABLE   1

/* Data Size Options */
#define USART_DATA_5BIT             0
#define USART_DATA_6BIT             1
#define USART_DATA_7BIT             2
#define USART_DATA_8BIT             3
#define USART_DATA_9BIT             7

/* Parity Mode Options */
#define USART_PARITY_DISABLE        0
#define USART_PARITY_EVEN           2
#define USART_PARITY_ODD            3

/* Stop Bit Options */
#define USART_STOP_1BIT             0
#define USART_STOP_2BIT             1

/* Operation Mode Options */
#define USART_ASYNCHRONOUS          0
#define USART_SYNCHRONOUS           1


/* =========================================================================
 * User Settings Selection
 * ========================================================================= */

/* System Clock Frequency */
#ifndef F_CPU
#define F_CPU                      11059200UL    /* 8 MHz default */
#endif

/* Desired Baud Rate */
#define USART_BAUD_RATE             115200UL

/* Select Double Speed Mode */
#define USART_DOUBLE_SPEED          USART_DOUBLE_SPEED_DISABLE

/* Select Data Frame Size */
#define USART_DATA_SIZE             USART_DATA_8BIT

/* Select Parity Option */
#define USART_PARITY_MODE           USART_PARITY_DISABLE

/* Select Stop Bits */
#define USART_STOP_BITS             USART_STOP_1BIT

/* Select Operation Mode */
#define USART_MODE                  USART_ASYNCHRONOUS

#endif /* MCAL_UART_UART_CONFIG_H_ */
