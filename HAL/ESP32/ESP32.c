/*
 * ESP32.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#include "ESP32_interface.h"
#include "ESP32_priv.h"
#include "ESP32_config.h"
#include "../../MCAL/UART/UART_interface.h"

static ESP32CoPro_ParserState_t ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
static ESP32CoPro_Command_t ESP32CoPro_RxFrame;
static uint8_t ESP32CoPro_RxIndex = 0;
static uint8_t ESP32CoPro_CommandReady = 0;
static ESP32CoPro_Command_t ESP32CoPro_LastCommand;

static uint8_t ESP32CoPro_u8CalculateCRC(uint8_t* copy_pu8Data, uint8_t copy_u8Length) {
    uint8_t local_u8CRC = 0;
    for (uint8_t i = 0; i < copy_u8Length; i++) {
        local_u8CRC ^= copy_pu8Data[i];
    }
    return local_u8CRC;
}

void ESP32CoPro_vidInit(void) {
    USRTInit();
    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
    ESP32CoPro_CommandReady = 0;
    ESP32CoPro_RxIndex = 0;
}

void ESP32CoPro_vidProcess(void) {
    while (USRTDataAvailable()) {
        uint8_t local_u8Byte = (uint8_t)USRTReadDataReg();

        switch (ESP32CoPro_ParserState) {
            case ESP32COPRO_STATE_WAIT_SOF:
                if (local_u8Byte == ESP32COPRO_SOF) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_CMD;
                }
                break;

            case ESP32COPRO_STATE_WAIT_CMD:
                ESP32CoPro_RxFrame.command = local_u8Byte;
                ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_LEN;
                break;

            case ESP32COPRO_STATE_WAIT_LEN:
                ESP32CoPro_RxFrame.length = local_u8Byte;
                ESP32CoPro_RxIndex = 0;
                if (ESP32CoPro_RxFrame.length == 0) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_CRC;
                } else if (ESP32CoPro_RxFrame.length > ESP32COPRO_MAX_PAYLOAD) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
                } else {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_PAYLOAD;
                }
                break;

            case ESP32COPRO_STATE_WAIT_PAYLOAD:
                ESP32CoPro_RxFrame.payload[ESP32CoPro_RxIndex++] = local_u8Byte;
                if (ESP32CoPro_RxIndex >= ESP32CoPro_RxFrame.length) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_CRC;
                }
                break;

            case ESP32COPRO_STATE_WAIT_CRC: {
                uint8_t local_u8Data[34];
                uint8_t local_u8Idx = 0;
                local_u8Data[local_u8Idx++] = ESP32CoPro_RxFrame.command;
                local_u8Data[local_u8Idx++] = ESP32CoPro_RxFrame.length;
                for (uint8_t i = 0; i < ESP32CoPro_RxFrame.length; i++) {
                    local_u8Data[local_u8Idx++] = ESP32CoPro_RxFrame.payload[i];
                }
                uint8_t local_u8ExpectedCRC = ESP32CoPro_u8CalculateCRC(local_u8Data, local_u8Idx);

                if (local_u8Byte == local_u8ExpectedCRC) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_EOF;
                } else {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
                }
                break;
            }

            case ESP32COPRO_STATE_WAIT_EOF:
                if (local_u8Byte == ESP32COPRO_EOF) {
                    ESP32CoPro_LastCommand = ESP32CoPro_RxFrame;
                    ESP32CoPro_CommandReady = 1;
                }
                ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
                break;

            default:
                ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
                break;
        }
    }
}

uint8_t ESP32CoPro_u8IsCommandAvailable(void) {
    return ESP32CoPro_CommandReady;
}

void ESP32CoPro_vidGetCommand(ESP32CoPro_Command_t* copy_pstCmd) {
    if (copy_pstCmd != NULL) {
        *copy_pstCmd = ESP32CoPro_LastCommand;
        ESP32CoPro_CommandReady = 0;
    }
}

static uint8_t ESP32CoPro_u8SendFrame(uint8_t copy_u8Cmd, uint8_t* copy_pu8Payload, uint8_t copy_u8Len) {
    uint8_t local_u8CRC;
    uint8_t local_u8Data[ESP32COPRO_MAX_PAYLOAD + 2];
    uint8_t i;

    if (copy_u8Len > ESP32COPRO_MAX_PAYLOAD) {
        return 1;
    }

    local_u8Data[0] = copy_u8Cmd;
    local_u8Data[1] = copy_u8Len;
    for (i = 0; i < copy_u8Len; i++) {
        local_u8Data[2 + i] = copy_pu8Payload[i];
    }
    local_u8CRC = ESP32CoPro_u8CalculateCRC(local_u8Data, copy_u8Len + 2);

    USRTSendDataSync((uint16_t)ESP32COPRO_SOF);
    USRTSendDataSync((uint16_t)copy_u8Cmd);
    USRTSendDataSync((uint16_t)copy_u8Len);
    for (i = 0; i < copy_u8Len; i++) {
        USRTSendDataSync((uint16_t)copy_pu8Payload[i]);
    }
    USRTSendDataSync((uint16_t)local_u8CRC);
    USRTSendDataSync((uint16_t)ESP32COPRO_EOF);

    return 0;
}

uint8_t ESP32CoPro_u8SendTelemetry(ESP32CoPro_Telemetry_t* copy_pstTelem) {
    uint8_t local_u8Payload[16];
    if (copy_pstTelem == NULL) return 1;

    local_u8Payload[0] = copy_pstTelem->speed;
    local_u8Payload[1] = (uint8_t)(copy_pstTelem->distance >> 8);
    local_u8Payload[2] = (uint8_t)(copy_pstTelem->distance);
    local_u8Payload[3] = copy_pstTelem->obstacle;
    local_u8Payload[4] = copy_pstTelem->lineState;
    local_u8Payload[5] = (uint8_t)(copy_pstTelem->imuAccelX >> 8);
    local_u8Payload[6] = (uint8_t)(copy_pstTelem->imuAccelX);
    local_u8Payload[7] = (uint8_t)(copy_pstTelem->imuAccelY >> 8);
    local_u8Payload[8] = (uint8_t)(copy_pstTelem->imuAccelY);
    local_u8Payload[9] = (uint8_t)(copy_pstTelem->imuAccelZ >> 8);
    local_u8Payload[10] = (uint8_t)(copy_pstTelem->imuAccelZ);
    local_u8Payload[11] = (uint8_t)(copy_pstTelem->imuGyroZ >> 8);
    local_u8Payload[12] = (uint8_t)(copy_pstTelem->imuGyroZ);

    return ESP32CoPro_u8SendFrame(ESP32COPRO_CMD_TELEMETRY, local_u8Payload, 13);
}

uint8_t ESP32CoPro_u8SendStatus(uint8_t copy_u8Mode, uint8_t copy_u8Errors) {
    uint8_t local_u8Payload[2];
    local_u8Payload[0] = copy_u8Mode;
    local_u8Payload[1] = copy_u8Errors;
    return ESP32CoPro_u8SendFrame(ESP32COPRO_CMD_STATUS, local_u8Payload, 2);
}

uint8_t ESP32CoPro_u8SendDebugString(uint8_t* copy_pu8Str, uint8_t copy_u8Len) {
    if (copy_pu8Str == NULL || copy_u8Len > ESP32COPRO_MAX_PAYLOAD) {
        return 1;
    }
    return ESP32CoPro_u8SendFrame(ESP32COPRO_CMD_DEBUG, copy_pu8Str, copy_u8Len);
}
