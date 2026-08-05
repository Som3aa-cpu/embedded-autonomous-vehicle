/*
 * UART_interface.h
 *
 *  Created on: 2 Aug 2026
 *      Author: mohib
 */

#ifndef MCAL_UART_UART_INTERFACE_H_
#define MCAL_UART_UART_INTERFACE_H_

#include "../../StdTypes.h"

void USRTInit(void);
void USRTEnableTxCH(void);
void USRTDisableTxCH(void);

void USRTEnableRxCH(void);
void USRTDisableRxCH(void);

void USRTEnableRXCompleteIreq(void);
void USRTDisableRXCompleteIreq(void);

void USRTEnableTXCompleteIreq(void);
void USRTDisableTXCompleteIreq(void);

void USRTEnableUDRemptyIreq(void);
void USRTDisableUDRemptyIreq(void);

void SetCallBackRX(PF CB);
void SetCallBackTC(PF CB);
void SetCallBackUDRemp(PF CB);

void USRTSendDataSync(uint16_t data);
uint16_t USRTReceiveDataSync(void);

uint16_t USRTReadDataReg(void);
void USRTWriteDataReg(uint16_t data);
uint8_t ReadErr(void);

#endif /* MCAL_UART_UART_INTERFACE_H_ */
