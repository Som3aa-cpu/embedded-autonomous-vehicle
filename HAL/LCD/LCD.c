/*
 * LCD.c
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#include "LCD_interface.h"
#include "LCD_config.h"
#include "LCD_priv.h"
#include "LCD_foonts.h"
#include "../../MCAL/I2C/I2C_interface.h"
#include "../../StdTypes.h"

static uint8_t SSD1306_u8ReadFlashByte(const uint8_t* Copy_pu8Addr)
{
    uint8_t Local_u8Result;

    __asm__ __volatile__
    (
        "lpm %0, Z"
        : "=r" (Local_u8Result)
        : "z" (Copy_pu8Addr)
    );

    return Local_u8Result;
}

/* Framebuffer: 128 columns x 8 pages (1 byte = 8 vertical pixels), page-major layout */
static uint8_t SSD1306_au8Buffer[SSD1306_WIDTH * SSD1306_PAGES];

static uint8_t SSD1306_u8WriteCommand(uint8_t Copy_u8Cmd)
{
    I2C_vidStartCond();
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_START)
    {
        return SSD1306_NOK;
    }

    I2C_vidSendData((uint8_t)(SSD1306_I2C_ADDRESS << 1) | 0U);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK)
    {
        return SSD1306_NOK;
    }

    I2C_vidSendData(SSD1306_CONTROL_CMD);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_DATA_ACK)
    {
        return SSD1306_NOK;
    }

    I2C_vidSendData(Copy_u8Cmd);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_DATA_ACK)
    {
        return SSD1306_NOK;
    }

    I2C_vidStopCond();
    return SSD1306_OK;
}

/* Streams Copy_u16Len bytes from Copy_pu8Data as display RAM data in one transaction */
static uint8_t SSD1306_u8WriteDataStream(const uint8_t* Copy_pu8Data, uint16_t Copy_u16Len)
{
    uint16_t Local_u16Index;

    I2C_vidStartCond();
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_START)
    {
        return SSD1306_NOK;
    }

    I2C_vidSendData((uint8_t)(SSD1306_I2C_ADDRESS << 1) | 0U);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK)
    {
        return SSD1306_NOK;
    }

    I2C_vidSendData(SSD1306_CONTROL_DATA);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_DATA_ACK)
    {
        return SSD1306_NOK;
    }

    for (Local_u16Index = 0U; Local_u16Index < Copy_u16Len; Local_u16Index++)
    {
        I2C_vidSendData(Copy_pu8Data[Local_u16Index]);
        I2C_vidWait();
        if (I2C_u8GetStatusCode() != MT_DATA_ACK)
        {
            return SSD1306_NOK;
        }
    }

    I2C_vidStopCond();
    return SSD1306_OK;
}

void SSD1306_vidInit(void)
{
    I2C_vidInit();
    I2C_vidEnable();

    SSD1306_u8WriteCommand(SSD1306_CMD_DISPLAY_OFF);

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_CLOCK_DIV);
    SSD1306_u8WriteCommand(0x80U);

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_MULTIPLEX);
    SSD1306_u8WriteCommand((uint8_t)(SSD1306_HEIGHT - 1U));

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_DISPLAY_OFFSET);
    SSD1306_u8WriteCommand(0x00U);

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_START_LINE | 0x00U);

    SSD1306_u8WriteCommand(SSD1306_CMD_CHARGE_PUMP);
    SSD1306_u8WriteCommand(0x14U);   /* internal charge pump enabled */

    SSD1306_u8WriteCommand(SSD1306_CMD_MEMORY_MODE);
    SSD1306_u8WriteCommand(0x00U);  /* horizontal addressing not used - page mode via SET_PAGE_ADDR */

    SSD1306_u8WriteCommand(SSD1306_CMD_SEGREMAP);       /* column 127 mapped to SEG0 */
    SSD1306_u8WriteCommand(SSD1306_CMD_COMSCANDEC);     /* remapped COM scan direction */

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_COM_PINS);
    SSD1306_u8WriteCommand(0x12U);

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_CONTRAST);
    SSD1306_u8WriteCommand(0xCFU);

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_PRECHARGE);
    SSD1306_u8WriteCommand(0xF1U);

    SSD1306_u8WriteCommand(SSD1306_CMD_SET_VCOM_DETECT);
    SSD1306_u8WriteCommand(0x40U);

    SSD1306_u8WriteCommand(SSD1306_CMD_DISPLAY_ALL_ON_RESUME);
    SSD1306_u8WriteCommand(SSD1306_CMD_NORMAL_DISPLAY);

    SSD1306_u8WriteCommand(SSD1306_CMD_DISPLAY_ON);

    SSD1306_vidClearBuffer();
    SSD1306_u8UpdateScreen();
}

void SSD1306_vidClearBuffer(void)
{
    uint16_t Local_u16Index;

    for (Local_u16Index = 0U; Local_u16Index < (SSD1306_WIDTH * SSD1306_PAGES); Local_u16Index++)
    {
        SSD1306_au8Buffer[Local_u16Index] = 0x00U;
    }
}

void SSD1306_vidSetPixel(uint8_t Copy_u8X, uint8_t Copy_u8Y, uint8_t Copy_u8Color)
{
    uint16_t Local_u16Index;
    uint8_t  Local_u8Bit;

    if ((Copy_u8X >= SSD1306_WIDTH) || (Copy_u8Y >= SSD1306_HEIGHT))
    {
        return;
    }

    Local_u16Index = (uint16_t)((Copy_u8Y / 8U) * SSD1306_WIDTH) + Copy_u8X;
    Local_u8Bit    = Copy_u8Y % 8U;

    if (Copy_u8Color == SSD1306_COLOR_WHITE)
    {
        SSD1306_au8Buffer[Local_u16Index] |= (uint8_t)(1U << Local_u8Bit);
    }
    else
    {
        SSD1306_au8Buffer[Local_u16Index] &= (uint8_t)~(1U << Local_u8Bit);
    }
}

void SSD1306_vidWriteChar(uint8_t Copy_u8X, uint8_t Copy_u8Page, char Copy_u8Char)
{
    uint8_t Local_u8Col;
    uint8_t Local_u8FontByte;
    uint16_t Local_u16Index;

    if ((Copy_u8Char < SSD1306_FONT_FIRST_CHAR) || (Copy_u8Char > SSD1306_FONT_LAST_CHAR))
    {
        Copy_u8Char = ' ';
    }

    if ((Copy_u8Page >= SSD1306_PAGES) || (Copy_u8X > (SSD1306_WIDTH - SSD1306_FONT_WIDTH)))
    {
        return;
    }

    for (Local_u8Col = 0U; Local_u8Col < SSD1306_FONT_WIDTH; Local_u8Col++)
    {
        Local_u8FontByte = SSD1306_u8ReadFlashByte(&OLED_Font5x7[(uint8_t)Copy_u8Char - SSD1306_FONT_FIRST_CHAR][Local_u8Col]);
        Local_u16Index   = (uint16_t)(Copy_u8Page * SSD1306_WIDTH) + Copy_u8X + Local_u8Col;
        SSD1306_au8Buffer[Local_u16Index] = Local_u8FontByte;
    }
}

void SSD1306_vidWriteString(uint8_t Copy_u8X, uint8_t Copy_u8Page, const char* Copy_pcStr)
{
    uint8_t Local_u8CursorX = Copy_u8X;

    while (*Copy_pcStr != '\0')
    {
        if (Local_u8CursorX > (SSD1306_WIDTH - SSD1306_FONT_WIDTH))
        {
            break; /* out of horizontal space on this line */
        }

        SSD1306_vidWriteChar(Local_u8CursorX, Copy_u8Page, *Copy_pcStr);
        Local_u8CursorX += (SSD1306_FONT_WIDTH + 1U); /* +1 column of spacing */
        Copy_pcStr++;
    }
}

uint8_t SSD1306_u8UpdateScreen(void)
{
    uint8_t Local_u8Page;

    for (Local_u8Page = 0U; Local_u8Page < SSD1306_PAGES; Local_u8Page++)
    {
        SSD1306_u8WriteCommand((uint8_t)(SSD1306_CMD_SET_PAGE_ADDR | Local_u8Page));
        SSD1306_u8WriteCommand(SSD1306_CMD_SET_LOW_COL | 0x00U);
        SSD1306_u8WriteCommand(SSD1306_CMD_SET_HIGH_COL | 0x00U);

        if (SSD1306_u8WriteDataStream(&SSD1306_au8Buffer[Local_u8Page * SSD1306_WIDTH], SSD1306_WIDTH) != SSD1306_OK)
        {
            return SSD1306_NOK;
        }
    }

    return SSD1306_OK;
}
