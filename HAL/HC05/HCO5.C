/*
 * HCO5.C
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */
#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "../../MCAL/UART/UART_interface.h"
#include "HC05_priv.h"
#include "HC05_config.h"
#include "HC05_interface.h"

static volatile uint8_t g_u8RxBuffer = 0;
static volatile uint8_t g_u8DataAvailableFlag = StdTypes_NOK;

void HC05_Init(void) {
    /* 1. Initialize low-level UART hardware */
    USRTInit();

    /* 2. Attach RX ISR callback */
    SetCallBackRX(HC05_voidRxCallback);

    /* 3. Enable RX Interrupts */
    USRTEnableRXCompleteIreq();
}

void HC05_SendChar(uint8_t Copy_u8Data) {
    USRTSendDataSync((uint16_t)Copy_u8Data);
}

void HC05_SendString(const uint8_t* Copy_pu8String) {
    if (Copy_pu8String != NULL) {
        uint32_t Local_u32Index = 0;
        while (Copy_pu8String[Local_u32Index] != '\0') {
            HC05_SendChar(Copy_pu8String[Local_u32Index]);
            Local_u32Index++;
        }
    }
}

void HC05_SendNumber(uint16_t Copy_u16Number) {
    uint8_t Local_u8Buffer[6];
    sint8_t Local_s8Index = 0;

    if (Copy_u16Number == 0) {
        HC05_SendChar('0');
        return;
    }

    while (Copy_u16Number > 0) {
        Local_u8Buffer[Local_s8Index] = (uint8_t)('0' + (Copy_u16Number % 10));
        Copy_u16Number /= 10;
        Local_s8Index++;
    }

    for (Local_s8Index = Local_s8Index - 1; Local_s8Index >= 0; Local_s8Index--) {
        HC05_SendChar(Local_u8Buffer[Local_s8Index]);
    }
}

uint8_t HC05_ReceiveCharSync(void) {
    return (uint8_t)USRTReceiveDataSync();
}

uint8_t HC05_GetReceivedCharAsync(void) {
    g_u8DataAvailableFlag = StdTypes_NOK;
    return g_u8RxBuffer;
}

uint8_t HC05_IsDataAvailable(void) {
    return g_u8DataAvailableFlag;
}

static void HC05_voidRxCallback(void) {
    g_u8RxBuffer = (uint8_t)USRTReadDataReg();
    g_u8DataAvailableFlag = StdTypes_OK;
}

