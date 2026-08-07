/*
 * EEPROM_interface.h
 *
 *  Created on: 7 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_EEPROM_EEPROM_INTERFACE_H_
#define HAL_EEPROM_EEPROM_INTERFACE_H_

#include "../../StdTypes.h"

/* Writes a single byte at MemAddress (0-1023).
 * Blocks internally until the EEPROM's internal write cycle completes
 * (ACK polling), so it's safe to call again immediately after.
 * Returns: EEPROM_OK / EEPROM_NOK */
uint8_t EEPROM_u8WriteByte(uint16_t Copy_u16MemAddress, uint8_t Copy_u8Data);

/* Reads a single byte from MemAddress (0-1023).
 * Returns: EEPROM_OK / EEPROM_NOK */
uint8_t EEPROM_u8ReadByte(uint16_t Copy_u16MemAddress, uint8_t* Copy_pu8Data);

/* Writes up to EEPROM_PAGE_SIZE bytes in one page-write transaction.
 * Copy_u8Len must not exceed EEPROM_PAGE_SIZE, and the write must not
 * cross a page boundary (call EEPROM_u8WriteBuffer for multi-page writes).
 * Blocks until the write cycle completes.
 * Returns: EEPROM_OK / EEPROM_NOK */
uint8_t EEPROM_u8WritePage(uint16_t Copy_u16MemAddress, const uint8_t* Copy_pu8Data, uint8_t Copy_u8Len);

/* Writes an arbitrary-length buffer, automatically splitting across
 * page boundaries as needed. Safe for any start address/length within
 * the EEPROM's range.
 * Returns: EEPROM_OK / EEPROM_NOK */
uint8_t EEPROM_u8WriteBuffer(uint16_t Copy_u16MemAddress, const uint8_t* Copy_pu8Data, uint16_t Copy_u16Len);

/* Sequentially reads Copy_u16Len bytes starting at MemAddress into Copy_pu8Buffer.
 * Returns: EEPROM_OK / EEPROM_NOK */
uint8_t EEPROM_u8ReadBuffer(uint16_t Copy_u16MemAddress, uint8_t* Copy_pu8Buffer, uint16_t Copy_u16Len);



#endif /* HAL_EEPROM_EEPROM_INTERFACE_H_ */
