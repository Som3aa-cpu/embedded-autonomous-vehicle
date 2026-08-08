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

/* Command IDs */
#define ESP32COPRO_CMD_DRIVE        0x01
#define ESP32COPRO_CMD_MODE         0x02
#define ESP32COPRO_CMD_CONFIG       0x03
#define ESP32COPRO_CMD_PING         0x04

#define ESP32COPRO_CMD_TELEMETRY    0x81
#define ESP32COPRO_CMD_STATUS       0x82
#define ESP32COPRO_CMD_DEBUG        0x83

/* Drive Directions */
#define ESP32COPRO_DIR_FWD          'F'
#define ESP32COPRO_DIR_BWD          'B'
#define ESP32COPRO_DIR_LEFT         'L'
#define ESP32COPRO_DIR_RIGHT        'R'
#define ESP32COPRO_DIR_STOP         'S'

/* Modes */
#define ESP32COPRO_MODE_MANUAL      0
#define ESP32COPRO_MODE_AUTO        1
#define ESP32COPRO_MODE_DIAG        2

/* Command Structure (received from ESP32) */
typedef struct {
    uint8_t command;
    uint8_t length;
    uint8_t payload[ESP32COPRO_MAX_PAYLOAD];
} ESP32CoPro_Command_t;

/* Telemetry Structure (sent to ESP32) */
typedef struct {
    uint8_t  speed;          /* 0-100 */
    uint16_t distance;       /* cm, 0-400 */
    uint8_t  obstacle;       /* 0 or 1 */
    uint8_t  lineState;      /* 5-bit sensor mask */
    sint16_t imuAccelX;      /* Scaled by 100 (e.g., 1.05g = 105) */
    sint16_t imuAccelY;
    sint16_t imuAccelZ;
    sint16_t imuGyroZ;       /* Yaw rate, deg/s scaled by 10 */
} ESP32CoPro_Telemetry_t;

/* Public API */
void ESP32CoPro_vidInit(void);
void ESP32CoPro_vidProcess(void);           /* Call every main loop iteration */
uint8_t   ESP32CoPro_uint8_tIsCommandAvailable(void);
void ESP32CoPro_vidGetCommand(ESP32CoPro_Command_t* copy_pstCmd);
uint8_t   ESP32CoPro_uint8_tSendTelemetry(ESP32CoPro_Telemetry_t* copy_pstTelem);
uint8_t   ESP32CoPro_uint8_tSendStatus(uint8_t copy_uint8_tMode, uint8_t copy_uint8_tErrors);
uint8_t   ESP32CoPro_uint8_tSendDebugString(uint8_t* copy_puint8_tStr, uint8_t copy_uint8_tLen);




#endif /* HAL_ESP32_ESP32_INTERFACE_H_ */
