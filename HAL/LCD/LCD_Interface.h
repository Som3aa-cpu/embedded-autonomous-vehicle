/*
 * LCD_Interface.h
 *
 *  Created on: 20 Jul 2026
 *      Author: mohib
 */

#ifndef HAL_LCD_INTERFACE_H_
#define HAL_LCD_INTERFACE_H_
#include "../../StdTypes.h"

#define LCD_8_BIT_MODE  1
#define LCD_4_BIT_MODE  0

#define LCD_LINE_ONE  1
#define LCD_LINE_TWO  2

#define LCD_SHIFT_LEFT 0
#define LCD_SHIFT_RIGHT 1

void LCD_vidInt(void);
void LCD_VideCmd( uint8_t copy_cmd);
void LCD_ViwriteChar(uint8_t copy_data);;
void LCD_writeHalfPort(uint8_t Value);
void LCD_writeString(uint8_t* String);
void LCD_writeNumber(uint8_t Number);
void LCD_clear(void);
void LCD_shift(uint8_t Direction);
void LCD_goToSpecific(uint8_t Row, uint8_t POS);
static void LCD_latchNibble(uint8_t copy_nibble);



#endif /* HAL_LCD_INTERFACE_H_ */
