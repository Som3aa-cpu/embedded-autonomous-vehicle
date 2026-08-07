/*
 * EEPROM_priv.h
 *
 *  Created on: 7 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_EEPROM_EEPROM_PRIV_H_
#define HAL_EEPROM_EEPROM_PRIV_H_

#define EEPROM_OK               0U
#define EEPROM_NOK               1U
/* Builds the 7-bit device address byte for a given 10-bit memory address.
 * AT24C08 packs the top 2 address bits (P1,P0) into the device address
 * itself: [1 0 1 0 A2 P1 P0] */
#define EEPROM_DEVICE_ADDR(MemAddr) \
    (uint8_t)(EEPROM_BASE_ADDRESS | (EEPROM_A2_STATE << 2) | (((MemAddr) >> 8) & 0x03U))

#define EEPROM_WORD_ADDR(MemAddr)  (uint8_t)((MemAddr) & 0xFFU)



#endif /* HAL_EEPROM_EEPROM_PRIV_H_ */
