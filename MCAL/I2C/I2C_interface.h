/*
 * I2C_interface.h
 *
 *  Created on: 3 Aug 2026
 *      Author: mohib
 */

#ifndef MCAL_I2C_I2C_INTERFACE_H_
#define MCAL_I2C_I2C_INTERFACE_H_
#include "../../StdTypes.h"

// Initialization & Control
void I2C_vidInit(void);
void I2C_vidEnable(void);
void I2C_vidDisable(void);

// Interrupt Control
void I2C_vidEnableIreq(void);
void I2C_vidDisableIreq(void);
void I2C_vidSetCallBack(PF pfCallbackCpy);

// Bus Conditions
void I2C_vidStartCond(void);
void I2C_vidRepStartCond(void);
void I2C_vidStopCond(void);

// Blocking Transmit/Receive
void I2C_vidSendData(uint8_t u8DataCpy);
uint8_t I2C_u8GetDataAck(void);
uint8_t I2C_u8GetDataNoAck(void);

// Non-blocking Receive (Interrupt Driven)
void I2C_vidStartReceive_ACK(void);
void I2C_vidStartReceive_N_ACK(void);
uint8_t I2C_u8ReadData(void);

uint8_t I2C_u8GetStatusCode(void);
void I2C_vidWait(void);

#define BUS_ERROR       0x00
#define MT_START        0x08
#define MT_REP_START    0x10
#define MT_SLAW_ACK     0x18
#define MT_SLAW_N_ACK   0x20
#define MT_DATA_ACK     0x28
#define MT_DATA_N_ACK   0x30
#define MT_LOSE_ARB     0x38
#define MR_SLAR_ACK     0x40
#define MR_SLAR_NACK    0x48
#define MR_DATA_ACK     0x50
#define MR_DATA_N_ACK   0x58
#define NO_INFO         0xF8

#endif /* MCAL_I2C_I2C_INTERFACE_H_ */
