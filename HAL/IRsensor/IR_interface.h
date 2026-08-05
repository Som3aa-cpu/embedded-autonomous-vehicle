/*
 * IR_interface.h
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_IRSENSOR_IR_INTERFACE_H_
#define HAL_IRSENSOR_IR_INTERFACE_H_
#include "../../StdTypes.h"

#define IR_CHANNEL_1          0  /* Far Left   */
#define IR_CHANNEL_2          1  /* Left       */
#define IR_CHANNEL_3          2  /* Center     */
#define IR_CHANNEL_4          3  /* Right      */
#define IR_CHANNEL_5          4  /* Far Right  */

/* Function Prototypes */
void IR_voidInit(void);
uint8_t   IR_u8ReadRaw(void);
uint8_t   IR_u8ReadChannel(uint8_t Copy_u8Channel);



#endif /* HAL_IRSENSOR_IR_INTERFACE_H_ */
