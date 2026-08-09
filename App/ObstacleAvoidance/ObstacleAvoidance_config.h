/*
 * ObstacleAvoidance_config.h
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#ifndef APP_OBSTACLEAVOIDANCE_OBSTACLEAVOIDANCE_CONFIG_H_
#define APP_OBSTACLEAVOIDANCE_OBSTACLEAVOIDANCE_CONFIG_H_

/* Distance threshold to trigger avoidance (cm) */
#define OBSTACLEAVOIDANCE_THRESHOLD_CM      20

/* Maneuver speeds */
#define OBSTACLEAVOIDANCE_BACKUP_SPEED      40
#define OBSTACLEAVOIDANCE_TURN_SPEED        50
#define OBSTACLEAVOIDANCE_FORWARD_SPEED     50

/*@
 * Durations in main-loop iterations.
 * Tune these based on your loop period (~1 ms typical).
 * Example: 300 iterations ≈ 300 ms
 */
#define OA_BACKUP_DURATION      300
#define OA_TURN_DURATION        400
#define OA_FORWARD_DURATION     600




#endif /* APP_OBSTACLEAVOIDANCE_OBSTACLEAVOIDANCE_CONFIG_H_ */
