/*
 * ObstacleAvoidance.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */


#include "ObstacleAvoidance_interface.h"
#include "ObstacleAvoidance_config.h"
#include "../../HAL/Motor/Motor_interface.h"
#include "../../HAL/HCSR04/HCSR04_interface.h"

typedef enum {
    OA_STATE_IDLE = 0,
    OA_STATE_STOP,
    OA_STATE_BACKUP,
    OA_STATE_TURN_AWAY,
    OA_STATE_FORWARD,
    OA_STATE_TURN_BACK,
    OA_STATE_DONE
} OA_State_t;

static OA_State_t OA_State = OA_STATE_IDLE;
static uint16_t OA_u16Counter = 0;

static void vidMotorForward(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void vidMotorBackward(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void vidMotorTurnRight(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void vidMotorTurnLeft(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void vidMotorStop(void) {
    Motor_StopAll();
}

void ObstacleAvoidance_vidInit(void) {
    OA_State = OA_STATE_IDLE;
    OA_u16Counter = 0;
}

uint8_t ObstacleAvoidance_u8IsObstacleDetected(void) {
    uint16_t u16Dist = HCSR04_u16GetDistanceCm();
    return ((u16Dist > 0) && (u16Dist < OBSTACLEAVOIDANCE_THRESHOLD_CM)) ? 1 : 0;
}

void ObstacleAvoidance_vidReset(void) {
    OA_State = OA_STATE_IDLE;
    OA_u16Counter = 0;
}

void ObstacleAvoidance_vidRun(void) {
    switch (OA_State) {
        case OA_STATE_IDLE:
            vidMotorStop();
            OA_State = OA_STATE_STOP;
            OA_u16Counter = 50;
            break;

        case OA_STATE_STOP:
            if (OA_u16Counter > 0) { OA_u16Counter--; break; }
            vidMotorBackward(OBSTACLEAVOIDANCE_BACKUP_SPEED);
            OA_State = OA_STATE_BACKUP;
            OA_u16Counter = OA_BACKUP_DURATION;
            break;

        case OA_STATE_BACKUP:
            if (OA_u16Counter > 0) { OA_u16Counter--; break; }
            vidMotorTurnRight(OBSTACLEAVOIDANCE_TURN_SPEED);
            OA_State = OA_STATE_TURN_AWAY;
            OA_u16Counter = OA_TURN_DURATION;
            break;

        case OA_STATE_TURN_AWAY:
            if (OA_u16Counter > 0) { OA_u16Counter--; break; }
            vidMotorForward(OBSTACLEAVOIDANCE_FORWARD_SPEED);
            OA_State = OA_STATE_FORWARD;
            OA_u16Counter = OA_FORWARD_DURATION;
            break;

        case OA_STATE_FORWARD:
            if (OA_u16Counter > 0) { OA_u16Counter--; break; }
            vidMotorTurnLeft(OBSTACLEAVOIDANCE_TURN_SPEED);
            OA_State = OA_STATE_TURN_BACK;
            OA_u16Counter = OA_TURN_DURATION;
            break;

        case OA_STATE_TURN_BACK:
            if (OA_u16Counter > 0) { OA_u16Counter--; break; }
            vidMotorStop();
            OA_State = OA_STATE_DONE;
            break;

        case OA_STATE_DONE:
            OA_State = OA_STATE_IDLE;
            break;

        default:
            OA_State = OA_STATE_IDLE;
            break;
    }
}
