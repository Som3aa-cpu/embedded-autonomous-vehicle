/*
 * VehicalControl.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#include "VehicaleControl_interface.h"
#include "VehicalControl_config.h"

#include "../../HAL/ESP32/ESP32_interface.h"
#include "../ModeManager/ModeManager_interface.h"
#include "../../HAL/Motor/Motor_interface.h"
#include "../../HAL/HCSR04/HCSR04_interface.h"
#include "../../HAL/IRsensor/IR_interface.h"

/*
 * NOTE: The function names below (Motor_xxx, Ultrasonic_xxx, etc.)
 * must match your existing HAL interface files.
 * Adjust if your actual HAL uses different naming.
 */

static ESP32CoPro_Telemetry_t VehicleControl_Telemetry;
static uint16_t VehicleControl_u16Tick = 0;

/* Private helpers */
static void VehicleControl_vidHandleCommand(ESP32CoPro_Command_t* pstCmd);
static void VehicleControl_vidUpdateTelemetry(void);
static void VehicleControl_vidExecuteDriveCommand(uint8_t u8Dir, uint8_t u8Speed);

void VehicleControl_vidInit(void) {
    /* Initialize wireless link and all hardware layers */
    ESP32CoPro_vidInit();
    ModeManager_vidInit();
    Motor_vidInit();
    Ultrasonic_vidInit();
    IRSensor_vidInit();

    VehicleControl_u16Tick = 0;

    /* Send initial status to phone */
    ESP32CoPro_u8SendStatus(ModeManager_u8GetMode(), 0);
}

void VehicleControl_vidRun(void) {
    /* 1. Parse incoming BLE/UART frames from ESP32 */
    ESP32CoPro_vidProcess();

    if (ESP32CoPro_u8IsCommandAvailable()) {
        ESP32CoPro_Command_t stCmd;
        ESP32CoPro_vidGetCommand(&stCmd);
        VehicleControl_vidHandleCommand(&stCmd);
    }

    /* 2. Run current mode logic (Manual/Auto/Diag) */
    ModeManager_vidRun();

    /* 3. Periodic telemetry to phone */
    VehicleControl_u16Tick++;
    if (VehicleControl_u16Tick >= VEHICLECONTROL_TELEM_PERIOD) {
        VehicleControl_u16Tick = 0;
        VehicleControl_vidUpdateTelemetry();
        ESP32CoPro_u8SendTelemetry(&VehicleControl_Telemetry);
    }
}

static void VehicleControl_vidHandleCommand(ESP32CoPro_Command_t* pstCmd) {
    if (pstCmd == NULL) {
        return;
    }

    switch (pstCmd->command) {
        case ESP32COPRO_CMD_DRIVE:
            /* Drive commands only valid in Manual mode */
            if (pstCmd->length >= 2 && ModeManager_u8GetMode() == MODE_MANUAL) {
            	uint8_t u8Dir = pstCmd->payload[0];
                uint8_t u8Speed = pstCmd->payload[1];
                VehicleControl_vidExecuteDriveCommand(u8Dir, u8Speed);
            }
            break;

        case ESP32COPRO_CMD_MODE:
            if (pstCmd->length >= 1) {
                ModeManager_vidSetMode(pstCmd->payload[0]);
                /* Acknowledge mode change back to phone */
                ESP32CoPro_u8SendStatus(ModeManager_u8GetMode(), 0);
            }
            break;

        case ESP32COPRO_CMD_PING:
            /* Heartbeat / link test */
            ESP32CoPro_u8SendStatus(ModeManager_u8GetMode(), 0);
            break;

        case ESP32COPRO_CMD_CONFIG:
            /* Reserved for future PID / calibration commands */
            break;

        default:
            /* Unknown command — ignore */
            break;
    }
}

static void VehicleControl_vidExecuteDriveCommand(uint8_t u8Dir, uint8_t u8Speed) {
    switch (u8Dir) {
        case ESP32COPRO_DIR_FWD:
            Motor_vidMoveForward(u8Speed);
            break;

        case ESP32COPRO_DIR_BWD:
            Motor_vidMoveBackward(u8Speed);
            break;

        case ESP32COPRO_DIR_LEFT:
            Motor_vidTurnLeft(u8Speed);
            break;

        case ESP32COPRO_DIR_RIGHT:
            Motor_vidTurnRight(u8Speed);
            break;

        case ESP32COPRO_DIR_STOP:
        default:
            Motor_vidStop();
            break;
    }
}

static void VehicleControl_vidUpdateTelemetry(void) {
    /* Speed: return current PWM or 0 if your HAL doesn't track it */
    VehicleControl_Telemetry.speed = Motor_u8GetCurrentSpeed();

    /* Ultrasonic distance and obstacle flag */
    VehicleControl_Telemetry.distance = Ultrasonic_u16GetDistance();
    VehicleControl_Telemetry.obstacle = Ultrasonic_u8IsObstacleDetected();

    /* Line sensor raw 5-bit value */
    VehicleControl_Telemetry.lineState = IRSensor_u8GetRawValue();

    /* IMU: populate when MPU6050 is integrated, else zero */
    VehicleControl_Telemetry.imuAccelX = 0;
    VehicleControl_Telemetry.imuAccelY = 0;
    VehicleControl_Telemetry.imuAccelZ = 0;
    VehicleControl_Telemetry.imuGyroZ  = 0;
}
