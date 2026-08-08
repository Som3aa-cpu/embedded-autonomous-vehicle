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

/* Private State */
static ESP32CoPro_ParserState_t ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
static ESP32CoPro_Command_t ESP32CoPro_RxFrame;
static uint8_t ESP32CoPro_RxIndex = 0;
static uint8_t ESP32CoPro_CommandReady = 0;
static ESP32CoPro_Command_t ESP32CoPro_LastCommand;

/* CRC8: Simple XOR */
static uint8_t ESP32CoPro_uint8_tCalculateCRC(uint8_t* copy_puint8_tData, uint8_t copy_uint8_tLength) {
    uint8_t local_uint8_tCRC = 0;
    for (uint8_t i = 0; i < copy_uint8_tLength; i++) {
        local_uint8_tCRC ^= copy_puint8_tData[i];
    }
    return local_uint8_tCRC;
}

void ESP32CoPro_vidInit(void) {
    USRTInit(); /* Baud rate fixed in UART_config.h at 115200 */
    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
    ESP32CoPro_CommandReady = 0;
    ESP32CoPro_RxIndex = 0;
}

/* State-machine parser � non-blocking, call frequently */
void ESP32CoPro_vidProcess(void) {
    while (USRTDataAvailable()) {
        uint8_t local_uint8_tByte = (uint8_t)USRTReadDataReg();

        switch (ESP32CoPro_ParserState) {
            case ESP32COPRO_STATE_WAIT_SOF:
                if (local_uint8_tByte == ESP32COPRO_SOF) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_CMD;
                }
                break;

            case ESP32COPRO_STATE_WAIT_CMD:
                ESP32CoPro_RxFrame.command = local_uint8_tByte;
                ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_LEN;
                break;

            case ESP32COPRO_STATE_WAIT_LEN:
                ESP32CoPro_RxFrame.length = local_uint8_tByte;
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
                ESP32CoPro_RxFrame.payload[ESP32CoPro_RxIndex++] = local_uint8_tByte;
                if (ESP32CoPro_RxIndex >= ESP32CoPro_RxFrame.length) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_CRC;
                }
                break;

            case ESP32COPRO_STATE_WAIT_CRC: {
                uint8_t local_uint8_tData[34];
                uint8_t local_uint8_tIdx = 0;
                local_uint8_tData[local_uint8_tIdx++] = ESP32CoPro_RxFrame.command;
                local_uint8_tData[local_uint8_tIdx++] = ESP32CoPro_RxFrame.length;
                for (uint8_t i = 0; i < ESP32CoPro_RxFrame.length; i++) {
                    local_uint8_tData[local_uint8_tIdx++] = ESP32CoPro_RxFrame.payload[i];
                }
                uint8_t local_uint8_tExpectedCRC = ESP32CoPro_uint8_tCalculateCRC(local_uint8_tData, local_uint8_tIdx);

                if (local_uint8_tByte == local_uint8_tExpectedCRC) {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_EOF;
                } else {
                    ESP32CoPro_ParserState = ESP32COPRO_STATE_WAIT_SOF;
                }
                break;
            }

            case ESP32COPRO_STATE_WAIT_EOF:
                if (local_uint8_tByte == ESP32COPRO_EOF) {
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

uint8_t ESP32CoPro_uint8_tIsCommandAvailable(void) {
    return ESP32CoPro_CommandReady;
}

void ESP32CoPro_vidGetCommand(ESP32CoPro_Command_t* copy_pstCmd) {
    if (copy_pstCmd != NULL) {
        *copy_pstCmd = ESP32CoPro_LastCommand;
        ESP32CoPro_CommandReady = 0;
    }
}

/* Internal: send framed packet to ESP32 */
static uint8_t ESP32CoPro_uint8_tSendFrame(uint8_t copy_uint8_tCmd, uint8_t* copy_puint8_tPayload, uint8_t copy_uint8_tLen) {
    uint8_t local_uint8_tCRC;
    uint8_t local_uint8_tData[ESP32COPRO_MAX_PAYLOAD + 2];
    uint8_t i;

    if (copy_uint8_tLen > ESP32COPRO_MAX_PAYLOAD) {
        return 1;
    }

    local_uint8_tData[0] = copy_uint8_tCmd;
    local_uint8_tData[1] = copy_uint8_tLen;
    for (i = 0; i < copy_uint8_tLen; i++) {
        local_uint8_tData[2 + i] = copy_puint8_tPayload[i];
    }
    local_uint8_tCRC = ESP32CoPro_uint8_tCalculateCRC(local_uint8_tData, copy_uint8_tLen + 2);

    USRTSendDataSync(ESP32COPRO_SOF);
    USRTSendDataSync(copy_uint8_tCmd);
    USRTSendDataSync(copy_uint8_tLen);
    for (i = 0; i < copy_uint8_tLen; i++) {
        USRTSendDataSync(copy_puint8_tPayload[i]);
    }
    USRTSendDataSync(local_uint8_tCRC);
    USRTSendDataSync(ESP32COPRO_EOF);

    return 0;
}

uint8_t ESP32CoPro_uint8_tSendTelemetry(ESP32CoPro_Telemetry_t* copy_pstTelem) {
    uint8_t local_uint8_tPayload[16];
    if (copy_pstTelem == NULL) return 1;

    local_uint8_tPayload[0] = copy_pstTelem->speed;
    local_uint8_tPayload[1] = (uint8_t)(copy_pstTelem->distance >> 8);
    local_uint8_tPayload[2] = (uint8_t)(copy_pstTelem->distance);
    local_uint8_tPayload[3] = copy_pstTelem->obstacle;
    local_uint8_tPayload[4] = copy_pstTelem->lineState;
    local_uint8_tPayload[5] = (uint8_t)(copy_pstTelem->imuAccelX >> 8);
    local_uint8_tPayload[6] = (uint8_t)(copy_pstTelem->imuAccelX);
    local_uint8_tPayload[7] = (uint8_t)(copy_pstTelem->imuAccelY >> 8);
    local_uint8_tPayload[8] = (uint8_t)(copy_pstTelem->imuAccelY);
    local_uint8_tPayload[9] = (uint8_t)(copy_pstTelem->imuAccelZ >> 8);
    local_uint8_tPayload[10] = (uint8_t)(copy_pstTelem->imuAccelZ);
    local_uint8_tPayload[11] = (uint8_t)(copy_pstTelem->imuGyroZ >> 8);
    local_uint8_tPayload[12] = (uint8_t)(copy_pstTelem->imuGyroZ);

    return ESP32CoPro_uint8_tSendFrame(ESP32COPRO_CMD_TELEMETRY, local_uint8_tPayload, 13);
}

uint8_t ESP32CoPro_uint8_tSendStatus(uint8_t copy_uint8_tMode, uint8_t copy_uint8_tErrors) {
    uint8_t local_uint8_tPayload[2];
    local_uint8_tPayload[0] = copy_uint8_tMode;
    local_uint8_tPayload[1] = copy_uint8_tErrors;
    return ESP32CoPro_uint8_tSendFrame(ESP32COPRO_CMD_STATUS, local_uint8_tPayload, 2);
}

uint8_t ESP32CoPro_uint8_tSendDebugString(uint8_t* copy_puint8_tStr, uint8_t copy_uint8_tLen) {
    if (copy_puint8_tStr == NULL || copy_uint8_tLen > ESP32COPRO_MAX_PAYLOAD) {
        return 1;
    }
    return ESP32CoPro_uint8_tSendFrame(ESP32COPRO_CMD_DEBUG, copy_puint8_tStr, copy_uint8_tLen);
}
