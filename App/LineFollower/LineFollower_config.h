/*
 * LineFollower_config.h
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#ifndef APP_LINEFOLLOWER_LINEFOLLOWER_CONFIG_H_
#define APP_LINEFOLLOWER_LINEFOLLOWER_CONFIG_H_

/* Base speed when centered on line (0–100) */
#define LINEFOLLOWER_BASE_SPEED         60

/* Speed for gentle turns */
#define LINEFOLLOWER_TURN_SPEED         45

/* Speed for sharp corrections */
#define LINEFOLLOWER_SHARP_TURN_SPEED   35

/* Stop if obstacle closer than this (cm) */
#define LINEFOLLOWER_OBSTACLE_THRESHOLD_CM  25

/*
 * Sensor bit mapping (5-bit raw value from IRSensor HAL):
 *   Bit 4 (PA4) = Far Left
 *   Bit 3 (PA3) = Left
 *   Bit 2 (PA2) = Center
 *   Bit 1 (PA1) = Right
 *   Bit 0 (PA0) = Far Right
 *
 * If your physical sensor array is mirrored, swap left/right
 * logic in LineFollower_program.c
 */


#endif /* APP_LINEFOLLOWER_LINEFOLLOWER_CONFIG_H_ */
