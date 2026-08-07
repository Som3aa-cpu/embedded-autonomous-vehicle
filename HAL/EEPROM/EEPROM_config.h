/*
 * EEPROM_config.h
 *
 *  Created on: 7 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_EEPROM_EEPROM_CONFIG_H_
#define HAL_EEPROM_EEPROM_CONFIG_H_

#define EEPROM_BASE_ADDRESS     0x50U

/* A2 pin state: 0 if tied to GND, 1 if tied to VCC */
#define EEPROM_A2_STATE         0U

#define EEPROM_SIZE_BYTES       1024U
#define EEPROM_PAGE_SIZE        16U

/* Typical AT24C08 internal write cycle time - used as a safety timeout
 * for ACK polling, not as a blind delay */
#define EEPROM_WRITE_CYCLE_MS   5U



#endif /* HAL_EEPROM_EEPROM_CONFIG_H_ */
