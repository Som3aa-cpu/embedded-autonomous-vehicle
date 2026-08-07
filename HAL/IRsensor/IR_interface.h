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

#define IRSENSOR_CHANNEL_0     0U
#define IRSENSOR_CHANNEL_1     1U
#define IRSENSOR_CHANNEL_2     2U
#define IRSENSOR_CHANNEL_3     3U
#define IRSENSOR_CHANNEL_4     4U

void IRSensor_voidInit(void);
uint8_t IRSensor_u8ReadChannel(uint8_t Copy_u8Channel);
uint8_t IRSensor_u8ReadRaw(void);


#endif /* HAL_IRSENSOR_IR_INTERFACE_H_ */
