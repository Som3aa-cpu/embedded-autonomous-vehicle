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


#define LED_BLUE_PORT   DIO_PORTA
#define LED_BLUE_PIN    DIO_PIN7

static ESP32CoPro_Telemetry_t VehicleControl_Telemetry;
static uint16_t VehicleControl_u16Tick = 0;
static uint8_t  VehicleControl_u8CurrentSpeed = 0;

static void VehicleControl_vidHandleCommand(ESP32CoPro_Command_t* pstCmd);
static void VehicleControl_vidUpdateTelemetry(void);
static void VehicleControl_vidExecuteDriveCommand(uint8_t u8Dir, uint8_t u8Speed);
static void VehicleControl_vidUpdateObstacleLED(void);

/* Motor convenience helpers */
static void vidMotorForward(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
    VehicleControl_u8CurrentSpeed = speed;
}

static void vidMotorBackward(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
    VehicleControl_u8CurrentSpeed = speed;
}

static void vidMotorTurnLeft(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
    VehicleControl_u8CurrentSpeed = speed;
}

static void vidMotorTurnRight(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
    VehicleControl_u8CurrentSpeed = speed;
}

static void vidMotorStop(void) {
    Motor_StopAll();
    VehicleControl_u8CurrentSpeed = 0;
}

void VehicleControl_vidInit(void) {
    ESP32CoPro_vidInit();
    ModeManager_vidInit();
    Motor_Init();
    HCSR04_voidInit();
    IRSensor_voidInit();

    VehicleControl_u16Tick = 0;
    VehicleControl_u8CurrentSpeed = 0;

    ESP32CoPro_u8SendStatus(ModeManager_u8GetMode(), 0);
}

void VehicleControl_vidRun(void) {
    ESP32CoPro_vidProcess();

    if (ESP32CoPro_u8IsCommandAvailable()) {
        ESP32CoPro_Command_t stCmd;
        ESP32CoPro_vidGetCommand(&stCmd);
        VehicleControl_vidHandleCommand(&stCmd);
    }

    ModeManager_vidRun();
    VehicleControl_vidUpdateObstacleLED();

    VehicleControl_u16Tick++;
    if (VehicleControl_u16Tick >= VEHICLECONTROL_TELEM_PERIOD) {
        VehicleControl_u16Tick = 0;
        VehicleControl_vidUpdateTelemetry();
        ESP32CoPro_u8SendTelemetry(&VehicleControl_Telemetry);
    }
}

static void VehicleControl_vidHandleCommand(ESP32CoPro_Command_t* pstCmd) {
    if (pstCmd == NULL) return;

    switch (pstCmd->command) {
        case ESP32COPRO_CMD_DRIVE:
            if (pstCmd->length >= 2 && ModeManager_u8GetMode() == MODE_MANUAL) {
                VehicleControl_vidExecuteDriveCommand(pstCmd->payload[0], pstCmd->payload[1]);
            }
            break;

        case ESP32COPRO_CMD_MODE:
            if (pstCmd->length >= 1) {
                ModeManager_vidSetMode(pstCmd->payload[0]);
                ESP32CoPro_u8SendStatus(ModeManager_u8GetMode(), 0);
            }
            break;

        case ESP32COPRO_CMD_PING:
            ESP32CoPro_u8SendStatus(ModeManager_u8GetMode(), 0);
            break;

        default:
            break;
    }
}

static void VehicleControl_vidExecuteDriveCommand(uint8_t u8Dir, uint8_t u8Speed) {
    switch (u8Dir) {
        case ESP32COPRO_DIR_FWD:  vidMotorForward(u8Speed);  break;
        case ESP32COPRO_DIR_BWD:  vidMotorBackward(u8Speed); break;
        case ESP32COPRO_DIR_LEFT: vidMotorTurnLeft(u8Speed);  break;
        case ESP32COPRO_DIR_RIGHT:vidMotorTurnRight(u8Speed); break;
        case ESP32COPRO_DIR_STOP:
        default:                  vidMotorStop();             break;
    }
}

static void VehicleControl_vidUpdateObstacleLED(void) {
    if (ModeManager_u8GetMode() == MODE_DIAGNOSTICS) return;

    uint8_t u8Obstacle = ObstacleAvoidance_u8IsObstacleDetected();
    DIO_setPinValue(LED_BLUE_PORT, LED_BLUE_PIN,
                    u8Obstacle ? DIO_PIN_HIGH : DIO_PIN_LOW);
}

static void VehicleControl_vidUpdateTelemetry(void) {
    VehicleControl_Telemetry.speed = VehicleControl_u8CurrentSpeed;
    VehicleControl_Telemetry.distance = HCSR04_u16GetDistanceCm();
    VehicleControl_Telemetry.obstacle = ObstacleAvoidance_u8IsObstacleDetected();
    VehicleControl_Telemetry.lineState = IRSensor_u8ReadRaw();
    VehicleControl_Telemetry.imuAccelX = 0;
    VehicleControl_Telemetry.imuAccelY = 0;
    VehicleControl_Telemetry.imuAccelZ = 0;
    VehicleControl_Telemetry.imuGyroZ  = 0;
}
