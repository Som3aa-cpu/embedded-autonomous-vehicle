/*
 * ESP32_interface.h
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_ESP32_ESP32_INTERFACE_H_
#define HAL_ESP32_ESP32_INTERFACE_H_
#include "../../StdTypes.h"

#define ESP32COPRO_SOF              0xAA
#define ESP32COPRO_EOF              0x55
#define ESP32COPRO_MAX_PAYLOAD      32

#define ESP32COPRO_CMD_DRIVE        0x01
#define ESP32COPRO_CMD_MODE         0x02
#define ESP32COPRO_CMD_CONFIG       0x03
#define ESP32COPRO_CMD_PING         0x04

#define ESP32COPRO_CMD_TELEMETRY    0x81
#define ESP32COPRO_CMD_STATUS       0x82
#define ESP32COPRO_CMD_DEBUG        0x83

#define ESP32COPRO_DIR_FWD          'F'
#define ESP32COPRO_DIR_BWD          'B'
#define ESP32COPRO_DIR_LEFT         'L'
#define ESP32COPRO_DIR_RIGHT        'R'
#define ESP32COPRO_DIR_STOP         'S'

#define ESP32COPRO_MODE_MANUAL      0
#define ESP32COPRO_MODE_AUTO        1
#define ESP32COPRO_MODE_DIAG        2

typedef struct {
    uint8_t command;
    uint8_t length;
    uint8_t payload[ESP32COPRO_MAX_PAYLOAD];
} ESP32CoPro_Command_t;

typedef struct {
    uint8_t  speed;
    uint16_t distance;
    uint8_t  obstacle;
    uint8_t  lineState;
    sint16_t imuAccelX;
    sint16_t imuAccelY;
    sint16_t imuAccelZ;
    sint16_t imuGyroZ;
} ESP32CoPro_Telemetry_t;

void ESP32CoPro_vidInit(void);
void ESP32CoPro_vidProcess(void);
uint8_t ESP32CoPro_u8IsCommandAvailable(void);
void ESP32CoPro_vidGetCommand(ESP32CoPro_Command_t* copy_pstCmd);
uint8_t ESP32CoPro_u8SendTelemetry(ESP32CoPro_Telemetry_t* copy_pstTelem);
uint8_t ESP32CoPro_u8SendStatus(uint8_t copy_u8Mode, uint8_t copy_u8Errors);
uint8_t ESP32CoPro_u8SendDebugString(uint8_t* copy_pu8Str, uint8_t copy_u8Len);





#endif /* HAL_ESP32_ESP32_INTERFACE_H_ */
