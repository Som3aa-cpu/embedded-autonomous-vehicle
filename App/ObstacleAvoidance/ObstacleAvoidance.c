/*
 * ObstacleAvoidance.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */


#include "ObstacleAvoidance_interface.h"
#include "ObstacleAvoidance_config.h"
#include "../../Motor/Motor_interface.h"
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

void ObstacleAvoidance_vidInit(void) {
    OA_State = OA_STATE_IDLE;
    OA_u16Counter = 0;
}

uint8_t ObstacleAvoidance_u8IsObstacleDetected(void) {
	uint16_t u16Dist = Ultrasonic_u16GetDistance();
    return ((u16Dist > 0) && (u16Dist < OBSTACLEAVOIDANCE_THRESHOLD_CM)) ? 1 : 0;
}

void ObstacleAvoidance_vidReset(void) {
    OA_State = OA_STATE_IDLE;
    OA_u16Counter = 0;
}

void ObstacleAvoidance_vidRun(void) {
    switch (OA_State) {
        case OA_STATE_IDLE:
            Motor_vidStop();
            OA_State = OA_STATE_STOP;
            OA_u16Counter = 50;     /* 50 ms stop */
            break;

        case OA_STATE_STOP:
            if (OA_u16Counter > 0) {
                OA_u16Counter--;
            } else {
                Motor_vidMoveBackward(OBSTACLEAVOIDANCE_BACKUP_SPEED);
                OA_State = OA_STATE_BACKUP;
                OA_u16Counter = OA_BACKUP_DURATION;
            }
            break;

        case OA_STATE_BACKUP:
            if (OA_u16Counter > 0) {
                OA_u16Counter--;
            } else {
                Motor_vidTurnRight(OBSTACLEAVOIDANCE_TURN_SPEED);
                OA_State = OA_STATE_TURN_AWAY;
                OA_u16Counter = OA_TURN_DURATION;
            }
            break;

        case OA_STATE_TURN_AWAY:
            if (OA_u16Counter > 0) {
                OA_u16Counter--;
            } else {
                Motor_vidMoveForward(OBSTACLEAVOIDANCE_FORWARD_SPEED);
                OA_State = OA_STATE_FORWARD;
                OA_u16Counter = OA_FORWARD_DURATION;
            }
            break;

        case OA_STATE_FORWARD:
            if (OA_u16Counter > 0) {
                OA_u16Counter--;
            } else {
                Motor_vidTurnLeft(OBSTACLEAVOIDANCE_TURN_SPEED);
                OA_State = OA_STATE_TURN_BACK;
                OA_u16Counter = OA_TURN_DURATION;
            }
            break;

        case OA_STATE_TURN_BACK:
            if (OA_u16Counter > 0) {
                OA_u16Counter--;
            } else {
                Motor_vidStop();
                OA_State = OA_STATE_DONE;
            }
            break;

        case OA_STATE_DONE:
            /* Maneuver complete — return to IDLE for next obstacle */
            OA_State = OA_STATE_IDLE;
            break;

        default:
            OA_State = OA_STATE_IDLE;
            break;
    }
}
