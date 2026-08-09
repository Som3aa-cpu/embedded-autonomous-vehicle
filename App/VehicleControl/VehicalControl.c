/*
 * VehicalControl.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#include "VehicaleControl_interface.h"
#include "VehicalControl_config.h"

#include "../../MCAL/DIO/DIO_Interface.h"
#include "../../HAL/ESP32/ESP32_interface.h"
#include "../ModeManager/ModeManager_interface.h"
#include "../../HAL/Motor/Motor_interface.h"
#include "../../HAL/HCSR04/HCSR04_interface.h"
#include "../../HAL/IRsensor/IR_interface.h"


/* LED Pin Definitions (from pin mapping) */
#define LED_BLUE_PORT   DIO_PORTA
#define LED_BLUE_PIN    DIO_PIN7

static ESP32CoPro_Telemetry_t VehicleControl_Telemetry;
static uint16_t VehicleControl_uint16_tTick = 0;

/* Private helpers */
static void VehicleControl_vidHandleCommand(ESP32CoPro_Command_t* pstCmd);
static void VehicleControl_vidUpdateTelemetry(void);
static void VehicleControl_vidExecuteDriveCommand(uint8_t uint8_tDir, uint8_t uint8_tSpeed);
static void VehicleControl_vidUpdateObstacleLED(void);

void VehicleControl_vidInit(void) {
    /* Initialize wireless link and all hardware layers */
    ESP32CoPro_vidInit();
    ModeManager_vidInit();
    Motor_vidInit();
    Ultrasonic_vidInit();
    IRSensor_vidInit();

    VehicleControl_uint16_tTick = 0;

    /* Send initial status to phone */
    ESP32CoPro_uint8_tSendStatus(ModeManager_uint8_tGetMode(), 0);
}

void VehicleControl_vidRun(void) {
    /* 1. Parse incoming BLE/UART frames from ESP32 */
    ESP32CoPro_vidProcess();

    if (ESP32CoPro_uint8_tIsCommandAvailable()) {
        ESP32CoPro_Command_t stCmd;
        ESP32CoPro_vidGetCommand(&stCmd);
        VehicleControl_vidHandleCommand(&stCmd);
    }

    /* 2. Run current mode logic (Manual/Auto/Diag) */
    ModeManager_vidRun();

    /* 3. Update obstacle warning LED (blue) */
    VehicleControl_vidUpdateObstacleLED();

    /* 4. Periodic telemetry to phone */
    VehicleControl_uint16_tTick++;
    if (VehicleControl_uint16_tTick >= VEHICLECONTROL_TELEM_PERIOD) {
        VehicleControl_uint16_tTick = 0;
        VehicleControl_vidUpdateTelemetry();
        ESP32CoPro_uint8_tSendTelemetry(&VehicleControl_Telemetry);
    }
}

static void VehicleControl_vidHandleCommand(ESP32CoPro_Command_t* pstCmd) {
    if (pstCmd == NULL) {
        return;
    }

    switch (pstCmd->command) {
        case ESP32COPRO_CMD_DRIVE:
            /* Drive commands only valid in Manual mode */
            if (pstCmd->length >= 2 && ModeManager_uint8_tGetMode() == MODE_MANUAL) {
                uint8_t uint8_tDir = pstCmd->payload[0];
                uint8_t uint8_tSpeed = pstCmd->payload[1];
                VehicleControl_vidExecuteDriveCommand(uint8_tDir, uint8_tSpeed);
            }
            break;

        case ESP32COPRO_CMD_MODE:
            if (pstCmd->length >= 1) {
                ModeManager_vidSetMode(pstCmd->payload[0]);
                /* Acknowledge mode change back to phone */
                ESP32CoPro_uint8_tSendStatus(ModeManager_uint8_tGetMode(), 0);
            }
            break;

        case ESP32COPRO_CMD_PING:
            /* Heartbeat / link test */
            ESP32CoPro_uint8_tSendStatus(ModeManager_uint8_tGetMode(), 0);
            break;

        case ESP32COPRO_CMD_CONFIG:
            /* Reserved for future PID / calibration commands */
            break;

        default:
            /* Unknown command — ignore */
            break;
    }
}

static void VehicleControl_vidExecuteDriveCommand(uint8_t uint8_tDir, uint8_t uint8_tSpeed) {
    switch (uint8_tDir) {
        case ESP32COPRO_DIR_FWD:
            Motor_vidMoveForward(uint8_tSpeed);
            break;

        case ESP32COPRO_DIR_BWD:
            Motor_vidMoveBackward(uint8_tSpeed);
            break;

        case ESP32COPRO_DIR_LEFT:
            Motor_vidTurnLeft(uint8_tSpeed);
            break;

        case ESP32COPRO_DIR_RIGHT:
            Motor_vidTurnRight(uint8_tSpeed);
            break;

        case ESP32COPRO_DIR_STOP:
        default:
            Motor_vidStop();
            break;
    }
}

/*
 * Blue LED = obstacle detected in Manual or Autonomous mode.
 * In Diagnostics mode, all LEDs are forced ON by ModeManager,
 * so we do not override here.
 */
static void VehicleControl_vidUpdateObstacleLED(void) {
    if (ModeManager_uint8_tGetMode() == MODE_DIAGNOSTICS) {
        return; /* ModeManager controls all LEDs in DIAG mode */
    }

    uint8_t uint8_tObstacle = ObstacleAvoidance_uint8_tIsObstacleDetected();
    DIO_setPinValue(LED_BLUE_PORT, LED_BLUE_PIN, uint8_tObstacle ? DIO_PIN_HIGH : DIO_PIN_LOW);
}

static void VehicleControl_vidUpdateTelemetry(void) {
    /* Speed: return current PWM or 0 if your HAL doesn't track it */
    VehicleControl_Telemetry.speed = Motor_uint8_tGetCurrentSpeed();

    /* Ultrasonic distance and obstacle flag */
    VehicleControl_Telemetry.distance = Ultrasonic_uint16_tGetDistance();
    VehicleControl_Telemetry.obstacle = ObstacleAvoidance_uint8_tIsObstacleDetected();

    /* Line sensor raw 5-bit value */
    VehicleControl_Telemetry.lineState = IRSensor_uint8_tGetRawValue();

    /* IMU: populate when MPU6050 is integrated, else zero */
    VehicleControl_Telemetry.imuAccelX = 0;
    VehicleControl_Telemetry.imuAccelY = 0;
    VehicleControl_Telemetry.imuAccelZ = 0;
    VehicleControl_Telemetry.imuGyroZ  = 0;
}
