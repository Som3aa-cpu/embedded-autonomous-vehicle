/*
 * EEPROM.c
 *
 *  Created on: 7 Aug 2026
 *      Author: mohib
 */

#include "EEPROM_interface.h"
#include "EEPROM_config.h"
#include "EEPROM_priv.h"
#include "../../MCAL/I2C/I2C_interface.h"

/* Polls the EEPROM with repeated START + device-address-write cycles
 * until it ACKs, meaning the internal write cycle has finished.
 * Bounded by a retry cap so a real fault can't hang forever. */
static uint8_t EEPROM_u8WaitReady(uint16_t Copy_u16MemAddress)
{
    uint16_t Local_u16Retries = 1000U;
    uint8_t  Local_u8Status;

    do
    {
        I2C_vidStartCond();
        Local_u8Status = I2C_u8GetStatusCode();
        if ((Local_u8Status != MT_START) && (Local_u8Status != MT_REP_START))
        {
            I2C_vidStopCond();
            return EEPROM_NOK;
        }

        I2C_vidSendData((uint8_t)(EEPROM_DEVICE_ADDR(Copy_u16MemAddress) << 1) | 0U);
        Local_u8Status = I2C_u8GetStatusCode();
        I2C_vidStopCond();

        Local_u16Retries--;
    } while ((Local_u8Status != MT_SLAW_ACK) && (Local_u16Retries > 0U));

    return (Local_u8Status == MT_SLAW_ACK) ? EEPROM_OK : EEPROM_NOK;
}

uint8_t EEPROM_u8WriteByte(uint16_t Copy_u16MemAddress, uint8_t Copy_u8Data)
{
    I2C_vidStartCond();
    if (I2C_u8GetStatusCode() != MT_START)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData((uint8_t)(EEPROM_DEVICE_ADDR(Copy_u16MemAddress) << 1) | 0U);
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData(EEPROM_WORD_ADDR(Copy_u16MemAddress));
    if (I2C_u8GetStatusCode() != MT_DATA_ACK)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData(Copy_u8Data);
    if (I2C_u8GetStatusCode() != MT_DATA_ACK)
    {
        return EEPROM_NOK;
    }

    I2C_vidStopCond();

    return EEPROM_u8WaitReady(Copy_u16MemAddress);
}

uint8_t EEPROM_u8ReadByte(uint16_t Copy_u16MemAddress, uint8_t* Copy_pu8Data)
{
    I2C_vidStartCond();
    if (I2C_u8GetStatusCode() != MT_START)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData((uint8_t)(EEPROM_DEVICE_ADDR(Copy_u16MemAddress) << 1) | 0U);
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData(EEPROM_WORD_ADDR(Copy_u16MemAddress));
    if (I2C_u8GetStatusCode() != MT_DATA_ACK)
    {
        return EEPROM_NOK;
    }

    I2C_vidRepStartCond();
    if (I2C_u8GetStatusCode() != MT_REP_START)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData((uint8_t)(EEPROM_DEVICE_ADDR(Copy_u16MemAddress) << 1) | 1U);
    if (I2C_u8GetStatusCode() != MR_SLAR_ACK)
    {
        return EEPROM_NOK;
    }

    *Copy_pu8Data = I2C_u8GetDataNoAck();
    I2C_vidStopCond();

    return EEPROM_OK;
}

uint8_t EEPROM_u8WritePage(uint16_t Copy_u16MemAddress, const uint8_t* Copy_pu8Data, uint8_t Copy_u8Len)
{
    uint8_t Local_u8Index;

    if ((Copy_u8Len == 0U) || (Copy_u8Len > EEPROM_PAGE_SIZE))
    {
        return EEPROM_NOK;
    }

    I2C_vidStartCond();
    if (I2C_u8GetStatusCode() != MT_START)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData((uint8_t)(EEPROM_DEVICE_ADDR(Copy_u16MemAddress) << 1) | 0U);
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK)
    {
        return EEPROM_NOK;
    }

    I2C_vidSendData(EEPROM_WORD_ADDR(Copy_u16MemAddress));
    if (I2C_u8GetStatusCode() != MT_DATA_ACK)
    {
        return EEPROM_NOK;
    }

    for (Local_u8Index = 0U; Local_u8Index < Copy_u8Len; Local_u8Index++)
    {
        I2C_vidSendData(Copy_pu8Data[Local_u8Index]);
        if (I2C_u8GetStatusCode() != MT_DATA_ACK)
        {
            return EEPROM_NOK;
        }
    }

    I2C_vidStopCond();

    return EEPROM_u8WaitReady(Copy_u16MemAddress);
}

uint8_t EEPROM_u8WriteBuffer(uint16_t Copy_u16MemAddress, const uint8_t* Copy_pu8Data, uint16_t Copy_u16Len)
{
    uint16_t Local_u16Remaining = Copy_u16Len;
    uint16_t Local_u16Addr      = Copy_u16MemAddress;
    const uint8_t* Local_pu8Ptr = Copy_pu8Data;
    uint8_t  Local_u8SpaceInPage;
    uint8_t  Local_u8Chunk;

    while (Local_u16Remaining > 0U)
    {
        Local_u8SpaceInPage = (uint8_t)(EEPROM_PAGE_SIZE - (Local_u16Addr % EEPROM_PAGE_SIZE));
        Local_u8Chunk = (Local_u16Remaining < Local_u8SpaceInPage) ? (uint8_t)Local_u16Remaining : Local_u8SpaceInPage;

        if (EEPROM_u8WritePage(Local_u16Addr, Local_pu8Ptr, Local_u8Chunk) != EEPROM_OK)
        {
            return EEPROM_NOK;
        }

        Local_u16Addr      += Local_u8Chunk;
        Local_pu8Ptr        += Local_u8Chunk;
        Local_u16Remaining -= Local_u8Chunk;
    }

    return EEPROM_OK;
}

uint8_t EEPROM_u8ReadBuffer(uint16_t Copy_u16MemAddress, uint8_t* Copy_pu8Buffer, uint16_t Copy_u16Len)
{
    uint16_t Local_u16Remaining = Copy_u16Len;
    uint16_t Local_u16Addr      = Copy_u16MemAddress;
    uint8_t* Local_pu8Ptr       = Copy_pu8Buffer;
    uint16_t Local_u16SpaceInBlock;
    uint16_t Local_u16Chunk;
    uint16_t Local_u16Index;

    /* The internal word-address counter is only 8 bits, so sequential
     * reads must be split at 256-byte block boundaries. */
    while (Local_u16Remaining > 0U)
    {
        Local_u16SpaceInBlock = 256U - (Local_u16Addr % 256U);
        Local_u16Chunk = (Local_u16Remaining < Local_u16SpaceInBlock) ? Local_u16Remaining : Local_u16SpaceInBlock;

        I2C_vidStartCond();
        if (I2C_u8GetStatusCode() != MT_START)
        {
            return EEPROM_NOK;
        }

        I2C_vidSendData((uint8_t)(EEPROM_DEVICE_ADDR(Local_u16Addr) << 1) | 0U);
        if (I2C_u8GetStatusCode() != MT_SLAW_ACK)
        {
            return EEPROM_NOK;
        }

        I2C_vidSendData(EEPROM_WORD_ADDR(Local_u16Addr));
        if (I2C_u8GetStatusCode() != MT_DATA_ACK)
        {
            return EEPROM_NOK;
        }

        I2C_vidRepStartCond();
        if (I2C_u8GetStatusCode() != MT_REP_START)
        {
            return EEPROM_NOK;
        }

        I2C_vidSendData((uint8_t)(EEPROM_DEVICE_ADDR(Local_u16Addr) << 1) | 1U);
        if (I2C_u8GetStatusCode() != MR_SLAR_ACK)
        {
            return EEPROM_NOK;
        }

        for (Local_u16Index = 0U; Local_u16Index < Local_u16Chunk; Local_u16Index++)
        {
            if (Local_u16Index < (Local_u16Chunk - 1U))
            {
                Local_pu8Ptr[Local_u16Index] = I2C_u8GetDataAck();
            }
            else
            {
                Local_pu8Ptr[Local_u16Index] = I2C_u8GetDataNoAck();
            }
        }

        I2C_vidStopCond();

        Local_u16Addr      += Local_u16Chunk;
        Local_pu8Ptr        += Local_u16Chunk;
        Local_u16Remaining -= Local_u16Chunk;
    }

    return EEPROM_OK;
}

