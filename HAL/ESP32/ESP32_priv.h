/*
 * ESP32_priv.h
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_ESP32_ESP32_PRIV_H_
#define HAL_ESP32_ESP32_PRIV_H_

#include "../../StdTypes.h"

#include "ESP32_interface.h"

typedef enum {
    ESP32COPRO_STATE_WAIT_SOF = 0,
    ESP32COPRO_STATE_WAIT_CMD,
    ESP32COPRO_STATE_WAIT_LEN,
    ESP32COPRO_STATE_WAIT_PAYLOAD,
    ESP32COPRO_STATE_WAIT_CRC,
    ESP32COPRO_STATE_WAIT_EOF
} ESP32CoPro_ParserState_t;

/* Private helpers */
static uint8_t ESP32CoPro_uint8_tCalculateCRC(uint8_t* copy_puint8_tData, uint8_t copy_uint8_tLength);
static void ESP32CoPro_vidProcessFrame(void);
static uint8_t ESP32CoPro_uint8_tSendFrame(uint8_t copy_uint8_tCmd, uint8_t* copy_puint8_tPayload, uint8_t copy_uint8_tLen);



#endif /* HAL_ESP32_ESP32_PRIV_H_ */
