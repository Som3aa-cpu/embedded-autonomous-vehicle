/*
 * LCD_priv.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_LCD_LCD_PRIV_H_
#define HAL_LCD_LCD_PRIV_H_

/* Control byte prefixes (I2C first byte after slave address) */
#define SSD1306_CONTROL_CMD        0x00U
#define SSD1306_CONTROL_DATA       0x40U

/* Commands */
#define SSD1306_CMD_DISPLAY_OFF            0xAEU
#define SSD1306_CMD_DISPLAY_ON             0xAFU
#define SSD1306_CMD_SET_CLOCK_DIV          0xD5U
#define SSD1306_CMD_SET_MULTIPLEX          0xA8U
#define SSD1306_CMD_SET_DISPLAY_OFFSET     0xD3U
#define SSD1306_CMD_SET_START_LINE         0x40U
#define SSD1306_CMD_CHARGE_PUMP            0x8DU
#define SSD1306_CMD_MEMORY_MODE            0x20U
#define SSD1306_CMD_SEGREMAP               0xA1U
#define SSD1306_CMD_COMSCANDEC              0xC8U
#define SSD1306_CMD_SET_COM_PINS           0xDAU
#define SSD1306_CMD_SET_CONTRAST           0x81U
#define SSD1306_CMD_SET_PRECHARGE          0xD9U
#define SSD1306_CMD_SET_VCOM_DETECT        0xDBU
#define SSD1306_CMD_DISPLAY_ALL_ON_RESUME  0xA4U
#define SSD1306_CMD_NORMAL_DISPLAY         0xA6U
#define SSD1306_CMD_INVERT_DISPLAY         0xA7U
#define SSD1306_CMD_SET_PAGE_ADDR          0xB0U   /* OR with page 0-7 */
#define SSD1306_CMD_SET_LOW_COL            0x00U   /* OR with low nibble */
#define SSD1306_CMD_SET_HIGH_COL           0x10U   /* OR with high nibble */

#define SSD1306_OK              0U
#define SSD1306_NOK             1U

#define SSD1306_FONT_WIDTH      5U
#define SSD1306_FONT_HEIGHT     7U
#define SSD1306_FONT_FIRST_CHAR 0x20U  /* space */
#define SSD1306_FONT_LAST_CHAR  0x7EU  /* ~ */



#endif /* HAL_LCD_LCD_PRIV_H_ */
