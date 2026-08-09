/*
 * ObstacleAvoidance_interface.h
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#ifndef APP_OBSTACLEAVOIDANCE_OBSTACLEAVOIDANCE_INTERFACE_H_
#define APP_OBSTACLEAVOIDANCE_OBSTACLEAVOIDANCE_INTERFACE_H_

#include "../../StdTypes.h"

void ObstacleAvoidance_vidInit(void);
void ObstacleAvoidance_vidRun(void);
void ObstacleAvoidance_vidReset(void);
uint8_t   ObstacleAvoidance_u8IsObstacleDetected(void);


#endif /* APP_OBSTACLEAVOIDANCE_OBSTACLEAVOIDANCE_INTERFACE_H_ */
