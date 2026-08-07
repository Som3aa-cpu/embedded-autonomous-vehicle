/*
 * LCD_config.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_LCD_LCD_CONFIG_H_
#define HAL_LCD_LCD_CONFIG_H_

/* 7-bit I2C slave address. Some modules use 0x3D instead - check your
 * module's silkscreen/datasheet if the display doesn't respond. */
#define SSD1306_I2C_ADDRESS    0x3C

#define SSD1306_WIDTH          128U
#define SSD1306_HEIGHT         64U
#define SSD1306_PAGES          (SSD1306_HEIGHT / 8U)   /* 8 pages */



#endif /* HAL_LCD_LCD_CONFIG_H_ */
