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

static uint8_t ESP32CoPro_u8CalculateCRC(uint8_t* copy_pu8Data, uint8_t copy_u8Length);
/* removed: ESP32CoPro_vidProcessFrame() — declared but never defined/used,
 * frame parsing is inlined directly in ESP32CoPro_vidProcess()'s switch statement */
static uint8_t ESP32CoPro_u8SendFrame(uint8_t copy_u8Cmd, uint8_t* copy_pu8Payload, uint8_t copy_u8Len);



#endif /* HAL_ESP32_ESP32_PRIV_H_ */
