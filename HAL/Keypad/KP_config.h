/*
 * KP_config.h
 *
 *  Created on: 21 Jul 2026
 *      Author: mohib
 */

#ifndef HAL_KP_CONFIG_H_
#define HAL_KP_CONFIG_H_
#include "../../StdTypes.h"

#define KEYPAD_ROWS    4
#define KEYPAD_COLS    4
extern const uint8_t Keypad_Map[KEYPAD_ROWS][KEYPAD_COLS];

#define KP_PORT		DIO_PORTA
#define KP_Col_start	DIO_PIN0    
#define KP_Col_end		DIO_PIN3
#define KP_Row_start	DIO_PIN4
#define KP_Row_end		DIO_PIN7

#endif /* HAL_KP_CONFIG_H_ */
