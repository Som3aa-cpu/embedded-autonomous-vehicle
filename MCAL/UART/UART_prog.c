/*
 * UART_prog.c
 *
 *  Created on: 2 Aug 2026
 *      Author: mohib
 */
#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "UART_reg.h"
#include "UART_config.h"

static PF pfnRXCallBack = NULL;
static PF pfnTXCallBack = NULL;
static PF pfnUDREmptyCallBack = NULL;

void USRTInit(void)
{
    uint16_t local_u16UBRRValue = 0;
    
    /* 1. Temporary variable for UCSRC. Bit 7 (URSEL) MUST be set to 1 */
    uint8_t local_u8UCSRCValue = (1 << 7); 
    
    /* 2. Double Speed Mode & UBRR Calculation */
#if (USART_DOUBLE_SPEED == USART_DOUBLE_SPEED_ENABLE)
    Set_Bit(UCSRA, 1); /* U2X = Bit 1 */
    local_u16UBRRValue = (uint16_t)(((F_CPU / (8UL * USART_BAUD_RATE))) - 1);
#else
    Clr_Bit(UCSRA, 1);
    local_u16UBRRValue = (uint16_t)(((F_CPU / (16UL * USART_BAUD_RATE))) - 1);
#endif

    /* 3. Character Size Selection (UCSZ0 & UCSZ1 in UCSRC, UCSZ2 in UCSRB) */
#if (USART_DATA_SIZE == USART_DATA_5BIT)
    Clr_Bit(UCSRB, 2);              /* UCSZ2 = 0 */
    Clr_Bit(local_u8UCSRCValue, 2); /* UCSZ1 = 0 */
    Clr_Bit(local_u8UCSRCValue, 1); /* UCSZ0 = 0 */
#elif (USART_DATA_SIZE == USART_DATA_6BIT)
    Clr_Bit(UCSRB, 2);
    Clr_Bit(local_u8UCSRCValue, 2);
    Set_Bit(local_u8UCSRCValue, 1);
#elif (USART_DATA_SIZE == USART_DATA_7BIT)
    Clr_Bit(UCSRB, 2);
    Set_Bit(local_u8UCSRCValue, 2);
    Clr_Bit(local_u8UCSRCValue, 1);
#elif (USART_DATA_SIZE == USART_DATA_8BIT)
    Clr_Bit(UCSRB, 2);
    Set_Bit(local_u8UCSRCValue, 2); /* UCSZ1 = 1 */
    Set_Bit(local_u8UCSRCValue, 1); /* UCSZ0 = 1 */
#elif (USART_DATA_SIZE == USART_DATA_9BIT)
    Set_Bit(UCSRB, 2);              /* UCSZ2 = 1 */
    Set_Bit(local_u8UCSRCValue, 2);
    Set_Bit(local_u8UCSRCValue, 1);
#endif

    /* 4. Operation Mode (UMSEL = Bit 6) */
#if (USART_MODE == USART_SYNCHRONOUS)
    Set_Bit(local_u8UCSRCValue, 6);
#else
    Clr_Bit(local_u8UCSRCValue, 6);
#endif

    /* 5. Parity Mode (UPM1 = Bit 5, UPM0 = Bit 4) */
#if (USART_PARITY_MODE == USART_PARITY_EVEN)
    Set_Bit(local_u8UCSRCValue, 5);
    Clr_Bit(local_u8UCSRCValue, 4);
#elif (USART_PARITY_MODE == USART_PARITY_ODD)
    Set_Bit(local_u8UCSRCValue, 5);
    Set_Bit(local_u8UCSRCValue, 4);
#else
    Clr_Bit(local_u8UCSRCValue, 5);
    Clr_Bit(local_u8UCSRCValue, 4);
#endif

    /* 6. Stop Bits Selection (USBS = Bit 3) */
#if (USART_STOP_BITS == USART_STOP_2BIT)
    Set_Bit(local_u8UCSRCValue, 3);
#else
    Clr_Bit(local_u8UCSRCValue, 3);
#endif

    /* 7. Single atomic write to UCSRC (URSEL=1 ensures it targets UCSRC) */
    UCSRC = local_u8UCSRCValue;

    /* 8. Load Baud Rate Registers (URSEL=0 bit ensures UBRRH is written) */
    UBRRL = (uint8_t)local_u16UBRRValue;
    UBRRH = (uint8_t)((local_u16UBRRValue >> 8) & 0x0F); /* URSEL (Bit 7) is implicitly 0 */

    /* 9. Enable Transmitter and Receiver in UCSRB */
    Set_Bit(UCSRB, 3); /* TXEN */
    Set_Bit(UCSRB, 4); /* RXEN */
}

void USRTEnableTxCH(void)
{
    Set_Bit(UCSRB, 3);
}

void USRTDisableTxCH(void)
{
    Clr_Bit(UCSRB, 3);
}

void USRTEnableRxCH(void)
{
    Set_Bit(UCSRB, 4);
}

void USRTDisableRxCH(void)
{
    Clr_Bit(UCSRB, 4);
}

void USRTEnableRXCompleteIreq(void)
{
    Set_Bit(UCSRB, 7);
}

void USRTDisableRXCompleteIreq(void)
{
    Clr_Bit(UCSRB, 7);
}

void USRTEnableTXCompleteIreq(void)
{
    Set_Bit(UCSRB, 6);
}

void USRTDisableTXCompleteIreq(void)
{
    Clr_Bit(UCSRB, 6);
}

void USRTEnableUDRemptyIreq(void)
{
    Set_Bit(UCSRB, 5);
}

void USRTDisableUDRemptyIreq(void)
{
    Clr_Bit(UCSRB, 5);
}

void SetCallBackRX(PF CB)
{
    pfnRXCallBack = CB;
}

void SetCallBackTC(PF CB)
{
    pfnTXCallBack = CB;
}

void SetCallBackUDRemp(PF CB)
{
    pfnUDREmptyCallBack = CB;
}

uint16_t USRTReadDataReg(void)
{
    uint16_t localData = 0;

#if (USART_DATA_SIZE == USART_DATA_9BIT)
    /* 1. Read RXB8 (Bit 1 of UCSRB) FIRST */
    uint8_t local_u8Bit8 = Get_Bit(UCSRB, 1);

    /* 2. Read UDR SECOND (Low 8 bits) */
    uint8_t local_u8LowByte = UDR;

    /* 3. Combine them into a single 16-bit integer */
    localData = (uint16_t)local_u8LowByte | ((uint16_t)local_u8Bit8 << 8);

#else
    /* For 5 to 8-bit modes, reading UDR directly is sufficient */
    localData = (uint16_t)UDR;
#endif

    return localData;
}

uint16_t USRTReceiveDataSync(void)
{
    /* Wait for Receive Complete Flag (RXC = Bit 7 in UCSRA) */
    while (Get_Bit(UCSRA, 7) == 0);

    /* Read and return data from registers */
    return USRTReadDataReg();
}

void USRTWriteDataReg(uint16_t data)
{
#if (USART_DATA_SIZE == USART_DATA_9BIT)
    /* 1. Extract the 9th bit (Bit 8) */
    uint8_t local_u8Bit8 = (uint8_t)((data >> 8) & 0x01);

    /* 2. Write the 9th bit to TXB8 (Bit 0 of UCSRB) BEFORE writing UDR */
    if (local_u8Bit8 == 1)
    {
        Set_Bit(UCSRB, 0); /* TXB8 = 1 */
    }
    else
    {
        Clr_Bit(UCSRB, 0); /* TXB8 = 0 */
    }

    /* 3. Load the lower 8 bits into UDR to trigger transmission */
    UDR = (uint8_t)(data & 0xFF);

#else
    /* For 5, 6, 7, or 8-bit data modes, write directly to UDR */
    UDR = (uint8_t)data;
#endif
}

void USRTSendDataSync(uint16_t data)
{
    while (Get_Bit(UCSRA, 5) == 0);

    USRTWriteDataReg(data);
}
uint8_t USRTDataAvailable(void)
{
    return Get_Bit(UCSRA, 7); /* RXC flag: 1 = data ready, 0 = empty */
}

uint8_t ReadErr(void)
{
    return ((UCSRA >> 2) & 0x07);
}

/* RX Complete ISR */
void __vector_13(void) __attribute__((signal));
void __vector_13(void)
{
    if (pfnRXCallBack != NULL)
    {
        pfnRXCallBack();
    }
}

/* UDR Empty ISR */
void __vector_14(void) __attribute__((signal));
void __vector_14(void)
{
    if (pfnUDREmptyCallBack != NULL)
    {
        pfnUDREmptyCallBack();
    }
}

/* TX Complete ISR */
void __vector_15(void) __attribute__((signal));
void __vector_15(void)
{
    if (pfnTXCallBack != NULL)
    {
        pfnTXCallBack();
    }
}
