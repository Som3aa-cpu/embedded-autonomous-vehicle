/*
 * EEPROM_interface.h
 *
 *  Created on: 4 Aug 2026
 *      Author: mohib
 */

#ifndef DEBUG_HAL_EEPROM_EEPROM_INTERFACE_H_
#define DEBUG_HAL_EEPROM_EEPROM_INTERFACE_H_
#include "../../StdTypes.h"

void eeprom_init(void);
void eeprom_write(uint8_t u8PageNumCpy, uint8_t u8ByteAddrCpy, uint8_t u8DataCpy);
uint8_t eeprom_read(uint8_t u8PageNumCpy, uint8_t u8ByteAddrCpy);

#endif /* DEBUG_HAL_EEPROM_EEPROM_INTERFACE_H_ */
