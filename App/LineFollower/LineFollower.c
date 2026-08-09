/*
 * LineFollower.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#include "LineFollower_interface.h"
#include "LineFollower_config.h"

#include "Motor_interface.h"
#include "../../HAL/IRsensor/IR_interface.h"
#include "../../HAL/HCSR04/HCSR04_interface.h"
#include "../ObsracalAvoidance/ObstacleAvoidance_interface.h"

void LineFollower_vidInit(void) {
    /* HAL modules already initialized by VehicleControl */
}

void LineFollower_vidRun(void) {
    uint16_t u16Distance = Ultrasonic_u16GetDistance();

    /* If obstacle detected, yield to avoidance routine */
    if ((u16Distance > 0) && (u16Distance < LINEFOLLOWER_OBSTACLE_THRESHOLD_CM)) {
        ObstacleAvoidance_vidRun();
        return;
    }

    uint8_t u8Line = IRSensor_u8GetRawValue();   /* 5-bit: PA4|PA3|PA2|PA1|PA0 */

    switch (u8Line) {
        /* Centered — go straight */
        case 0b00100:
            Motor_vidMoveForward(LINEFOLLOWER_BASE_SPEED);
            break;

        /* Slight left drift */
        case 0b01100:   /* Center + Left */
        case 0b01000:   /* Left only */
            Motor_vidTurnLeft(LINEFOLLOWER_TURN_SPEED);
            break;

        /* Slight right drift */
        case 0b00110:   /* Center + Right */
        case 0b00010:   /* Right only */
            Motor_vidTurnRight(LINEFOLLOWER_TURN_SPEED);
            break;

        /* Sharp left correction */
        case 0b10000:   /* Far left */
            Motor_vidTurnLeft(LINEFOLLOWER_SHARP_TURN_SPEED);
            break;

        /* Sharp right correction */
        case 0b00001:   /* Far right */
            Motor_vidTurnRight(LINEFOLLOWER_SHARP_TURN_SPEED);
            break;

        /* Lost line — all white */
        case 0b00000:
            Motor_vidStop();
            break;

        /* Cross line / wide line — all black */
        case 0b11111:
            Motor_vidMoveForward(LINEFOLLOWER_BASE_SPEED);
            break;

        /* Transition states — pick dominant side */
        default:
            if (u8Line & 0b11000) {         /* Left side active */
                Motor_vidTurnLeft(LINEFOLLOWER_TURN_SPEED);
            }
            else if (u8Line & 0b00011) {    /* Right side active */
                Motor_vidTurnRight(LINEFOLLOWER_TURN_SPEED);
            }
            else {
                Motor_vidMoveForward(LINEFOLLOWER_BASE_SPEED);
            }
            break;
    }
}
