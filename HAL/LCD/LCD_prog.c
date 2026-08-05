/*
 * LCD_prog.c
 *
 *  Created on: 20 Jul 2026
 *      Author: mohib
 */

#include <stdlib.h>
#include <util/delay.h>

#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "../../MCAL/DIO/DIO_Interface.h"
#include "../../MCAL/DIO/DIO_REG.h"

#include "LCD_Interface.h"
#include "LCD_config.h"
#include "LCD_cmd.h"


static void LCD_latchNibble(uint8_t copy_nibble)
{
    LCD_writeHalfPort(copy_nibble);
    DIO_setPinValue(LCD_EN_PORT, LCD_EN_PIN, DIO_PIN_HIGH);
    _delay_us(1);
    DIO_setPinValue(LCD_EN_PORT, LCD_EN_PIN, DIO_PIN_LOW);
    _delay_us(200);
}
void LCD_vidInt(void)
{
#if LCD_MODE == 8
    // Configure full 8-bit data port as output
    DIO_setPortDirection(LCD_DATA_PORT, DIO_PORT_OUTPUT);
#elif LCD_MODE == 4
    DIO_setPinValue(LCD_RS_PORT, LCD_RS_PIN, DIO_PIN_LOW);
        DIO_setPinValue(LCD_RW_PORT, LCD_RW_PIN, DIO_PIN_LOW);

        // 4-bit initialization wake-up sequence
        LCD_latchNibble(0x03);
        _delay_ms(5);

        LCD_latchNibble(0x03);
        _delay_us(150);

        LCD_latchNibble(0x03);
        _delay_us(150);

        // Force 4-bit mode interface (sets LCD to read 4-bit transfers)
        LCD_latchNibble(0x02);
        _delay_ms(2);

        // Now safe to send full commands using high/low nibbles
        LCD_VideCmd(Function_Set4_bit_cmd); // N=1 (2 lines), F=0 (5x7 font) -> usually 0x28
        _delay_ms(1);

        LCD_VideCmd(Display_ON_cmd);        // Display ON, Cursor OFF (0x0C)
        _delay_ms(1);

        LCD_VideCmd(Clear_Display_cmd);     // Clear display (0x01)
        _delay_ms(2);

        LCD_VideCmd(Entry_Mode_cmd);        // Auto-increment (0x06)
#endif

    // Configure control pins as output
    DIO_setPinDirection(LCD_RS_PORT, LCD_RS_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(LCD_RW_PORT, LCD_RW_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(LCD_EN_PORT, LCD_EN_PIN, DIO_PIN_OUTPUT);

    _delay_ms(50); // Power-on delay (>15ms required by HD44780)

#if LCD_MODE == 8
    LCD_VideCmd(Function_Set_cmd);      // 8-bit mode, 2 lines, 5x7 dots
    _delay_ms(1);
    LCD_VideCmd(Display_ON_cmd);        // Display ON, Cursor OFF
    _delay_ms(1);
    LCD_VideCmd(Clear_Display_cmd);     // Clear display
    _delay_ms(2);
    LCD_VideCmd(Entry_Mode_cmd);        // Auto-increment cursor

#elif LCD_MODE == 4
    // 4-bit initialization sequence
    LCD_writeHalfPort(0x03);
    _delay_ms(5);
    LCD_writeHalfPort(0x03);
    _delay_us(150);
    LCD_writeHalfPort(0x03);
    LCD_writeHalfPort(0x02); // Select 4-bit mode

    LCD_VideCmd(Function_Set4_bit_cmd);
    _delay_ms(1);
    LCD_VideCmd(Display_ON_cmd);
    _delay_ms(1);
    LCD_VideCmd(Clear_Display_cmd);
    _delay_ms(2);
    LCD_VideCmd(Entry_Mode_cmd);
#else
#error "Invalid LCD mode"
#endif
}

void LCD_VideCmd(uint8_t copy_cmd)
{
	DIO_setPinValue(LCD_RS_PORT, LCD_RS_PIN, DIO_PIN_LOW);
	    DIO_setPinValue(LCD_RW_PORT, LCD_RW_PIN, DIO_PIN_LOW);

	#if LCD_MODE == 8
	    DIO_setPortValue(LCD_DATA_PORT, copy_cmd);
	    DIO_setPinValue(LCD_EN_PORT, LCD_EN_PIN, DIO_PIN_HIGH);
	    _delay_us(1);
	    DIO_setPinValue(LCD_EN_PORT, LCD_EN_PIN, DIO_PIN_LOW);
	    _delay_ms(2);

	#elif LCD_MODE == 4
	    LCD_latchNibble(copy_cmd >> 4); // Send High Nibble
	    LCD_latchNibble(copy_cmd);      // Send Low Nibble
	    _delay_ms(2);
	#endif
}

void LCD_ViwriteChar(uint8_t copy_data)
{
	DIO_setPinValue(LCD_RS_PORT, LCD_RS_PIN, DIO_PIN_HIGH);
	    DIO_setPinValue(LCD_RW_PORT, LCD_RW_PIN, DIO_PIN_LOW);

	#if LCD_MODE == 8
	    DIO_setPortValue(LCD_DATA_PORT, copy_data);
	    DIO_setPinValue(LCD_EN_PORT, LCD_EN_PIN, DIO_PIN_HIGH);
	    _delay_us(1);
	    DIO_setPinValue(LCD_EN_PORT, LCD_EN_PIN, DIO_PIN_LOW);
	    _delay_ms(2);

	#elif LCD_MODE == 4
	    LCD_latchNibble(copy_data >> 4); // Send High Nibble
	    LCD_latchNibble(copy_data);      // Send Low Nibble
	    _delay_ms(2);
	#endif
}

void LCD_writeHalfPort(uint8_t Value)
{
    DIO_setPinValue(LCD_DATA_PORT, LCD_D4_PIN, Get_Bit(Value, 0));
    DIO_setPinValue(LCD_DATA_PORT, LCD_D5_PIN, Get_Bit(Value, 1));
    DIO_setPinValue(LCD_DATA_PORT, LCD_D6_PIN, Get_Bit(Value, 2));
    DIO_setPinValue(LCD_DATA_PORT, LCD_D7_PIN, Get_Bit(Value, 3));
}

void LCD_writeString(uint8_t* String)
{
    uint8_t Local_Index = 0;

    while (String[Local_Index] != '\0')
    {
        LCD_ViwriteChar(String[Local_Index]);
        Local_Index++;
    }
}

void LCD_writeNumber(uint8_t Number)
{
    char Local_str[12];

    itoa(Number, Local_str, 10);
    LCD_writeString((uint8_t*)Local_str);
}
void LCD_clear(void)
{
    LCD_VideCmd(Clear_Display_cmd);
    _delay_ms(2);
}

void LCD_shift(uint8_t Direction)
{
    switch (Direction)
    {
    case LCD_SHIFT_LEFT:
        LCD_VideCmd(0x18);
        break;

    case LCD_SHIFT_RIGHT:
        LCD_VideCmd(0x1C);
        break;

    default:
        break;
    }
}

void LCD_goToSpecific(uint8_t Row, uint8_t POS)
{
    uint8_t Local_u8Address = 0;

    switch (Row)
    {
    case 0:
        Local_u8Address = POS;
        break;
    case 1:
        Local_u8Address = POS + 0x40;
        break;
    case 2:
        Local_u8Address = POS + 0x14; // Line 3 DDRAM offset
        break;
    case 3:
        Local_u8Address = POS + 0x54; // Line 4 DDRAM offset
        break;
    default:
        return;
    }


    LCD_VideCmd(0x80 | Local_u8Address);
}
