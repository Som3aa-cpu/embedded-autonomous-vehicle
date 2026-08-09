/*
 * LineFollower.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#include "LineFollower_interface.h"
#include "LineFollower_config.h"

#include "../../HAL/Motor/Motor_interface.h"
#include "../../HAL/IRsensor/IR_interface.h"
#include "../../HAL/HCSR04/HCSR04_interface.h"
#include "../ObstacleAvoidance/ObstacleAvoidance_interface.h"

static void vidMotorForward(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void vidMotorTurnLeft(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void vidMotorTurnRight(uint8_t speed) {
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void vidMotorStop(void) {
    Motor_StopAll();
}

void LineFollower_vidInit(void) {
}

void LineFollower_vidRun(void) {
    uint16_t u16Distance = HCSR04_u16GetDistanceCm();

    if ((u16Distance > 0) && (u16Distance < LINEFOLLOWER_OBSTACLE_THRESHOLD_CM)) {
        ObstacleAvoidance_vidRun();
        return;
    }

    uint8_t u8Line = IRSensor_u8ReadRaw();

    switch (u8Line) {
        case 0b00100:
            vidMotorForward(LINEFOLLOWER_BASE_SPEED);
            break;
        case 0b01100:
        case 0b01000:
            vidMotorTurnLeft(LINEFOLLOWER_TURN_SPEED);
            break;
        case 0b00110:
        case 0b00010:
            vidMotorTurnRight(LINEFOLLOWER_TURN_SPEED);
            break;
        case 0b10000:
        case 0b11000:
            vidMotorTurnLeft(LINEFOLLOWER_SHARP_TURN_SPEED);
            break;
        case 0b00001:
        case 0b00011:
            vidMotorTurnRight(LINEFOLLOWER_SHARP_TURN_SPEED);
            break;
        case 0b11111:
            vidMotorForward(LINEFOLLOWER_BASE_SPEED);
            break;
        case 0b00000:
        default:
            vidMotorStop();
            break;
    }
}
