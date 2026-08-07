/*
 * LCD_interface.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_LCD_LCD_INTERFACE_H_
#define HAL_LCD_LCD_INTERFACE_H_

#include "../../StdTypes.h"

#define SSD1306_COLOR_BLACK    0U
#define SSD1306_COLOR_WHITE    1U

/* Runs the SSD1306 power-on init sequence. Call once at startup. */
void SSD1306_vidInit(void);

/* Clears the in-RAM framebuffer (does not touch the physical display until
 * SSD1306_vidUpdateScreen() is called) */
void SSD1306_vidClearBuffer(void);

/* Sets a single pixel in the framebuffer. x: 0-127, y: 0-63 */
void SSD1306_vidSetPixel(uint8_t Copy_u8X, uint8_t Copy_u8Y, uint8_t Copy_u8Color);

/* Draws a single character into the framebuffer at (x, page).
 * page: 0-7 (each page is 8 pixel rows tall) */
void SSD1306_vidWriteChar(uint8_t Copy_u8X, uint8_t Copy_u8Page, char Copy_u8Char);

/* Draws a null-terminated string starting at (x, page), advancing 6px per char */
void SSD1306_vidWriteString(uint8_t Copy_u8X, uint8_t Copy_u8Page, const char* Copy_pcStr);

/* Flushes the framebuffer to the physical display over I2C.
 * Returns: SSD1306_OK / SSD1306_NOK */
uint8_t SSD1306_u8UpdateScreen(void);



#endif /* HAL_LCD_LCD_INTERFACE_H_ */
