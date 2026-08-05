/*
 * HC05_interface.h
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_HC05_HC05_INTERFACE_H_
#define HAL_HC05_HC05_INTERFACE_H_
#include "../../StdTypes.h"


void HC05_Init(void);

void HC05_SendChar(uint8_t Copy_u8Data);

void HC05_SendString(const uint8_t* Copy_pu8String);

void HC05_SendNumber(uint16_t Copy_u16Number);

uint8_t HC05_ReceiveCharSync(void);

uint8_t HC05_GetReceivedCharAsync(void);

uint8_t HC05_IsDataAvailable(void);



#endif /* HAL_HC05_HC05_INTERFACE_H_ */
