/*
 * EEPROM.c
 *
 *  Created on: 4 Aug 2026
 *      Author: mohib
 */
#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include <util/delay.h>
#include "EEPROM_config.h"
#include "EEPROM_priv.h"
#include "EEPROM_interface.h"
#include "../../MCAL/I2C/I2C_interface.h"

void eeprom_init(void)
{
	I2C_vidInit();
	I2C_vidEnable();
}
void eeprom_write(uint8_t u8PageNumCpy, uint8_t u8ByteAddrCpy, uint8_t u8DataCpy)
{
    I2C_vidStartCond();

    if (I2C_u8GetStatusCode() == MT_START)
    {
        I2C_vidSendData(0b10100000 | (u8PageNumCpy << 1));

        if (I2C_u8GetStatusCode() == MT_SLAW_ACK)
        {
            I2C_vidSendData(u8ByteAddrCpy);

            if (I2C_u8GetStatusCode() == MT_DATA_ACK)
            {
                I2C_vidSendData(u8DataCpy);

                if (I2C_u8GetStatusCode() == MT_DATA_ACK)
                {
                    I2C_vidStopCond();
                }
            }
        }
    }
}
uint8_t eeprom_read(uint8_t u8PageNumCpy, uint8_t u8ByteAddrCpy)
{
uint8_t u8DataCpy = 0;

    I2C_vidStartCond();

    if (I2C_u8GetStatusCode() == MT_START)
    {
        I2C_vidSendData(0b10100000 | (u8PageNumCpy << 1));

        if (I2C_u8GetStatusCode() == MT_SLAW_ACK)
        {
            I2C_vidSendData(u8ByteAddrCpy);
            if (I2C_u8GetStatusCode() == MT_DATA_ACK)
            {
                I2C_vidRepStartCond();

                if (I2C_u8GetStatusCode() == MT_REP_START)
                {
                    I2C_vidSendData(0b10100001 | (u8PageNumCpy << 1));
                    if (I2C_u8GetStatusCode() == MR_SLAR_ACK)
                    {
                        u8DataCpy = I2C_u8GetDataNoAck();

                        I2C_vidStopCond();
                    }
                }
            }
        }
    }
    return u8DataCpy;
}
