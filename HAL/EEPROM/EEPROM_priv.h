/*
 * EEPROM_priv.h
 *
 *  Created on: 4 Aug 2026
 *      Author: mohib
 */

#ifndef DEBUG_HAL_EEPROM_EEPROM_PRIV_H_
#define DEBUG_HAL_EEPROM_EEPROM_PRIV_H_
#include "EEPROM_config.h"

static inline uint8_t EEPROM_u8GetSlaveAddress(uint16_t u16MemAddr, uint8_t u8ReadBit) {
    uint8_t u8Sla = 0;

    u8Sla |= (0b1010 << 4);
#if (EEPROM_TYPE == EEPROM_TYPE_24C08)
    u8Sla |= ((EEPROM_A2_PIN_STATE & 0x01) << 3);
    u8Sla |= ((uint8_t)((u16MemAddr >> 8) & 0x03) << 1);
#elif (EEPROM_TYPE == EEPROM_TYPE_24C16)
    u8Sla |= ((uint8_t)((u16MemAddr >> 8) & 0x07) << 1);
#endif

    u8Sla |= (u8ReadBit & 0x01);
    return u8Sla;
}
static void EEPROM_vidWritePageInternal(uint16_t u16MemAddr, const uint8_t *pu8Data, uint16_t u16Length);


#endif /* DEBUG_HAL_EEPROM_EEPROM_PRIV_H_ */
