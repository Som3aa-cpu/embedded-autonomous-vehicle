/*
 * ModeManger_interface.h
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#ifndef APP_MODEMANAGER_MODEMANAGER_INTERFACE_H_
#define APP_MODEMANAGER_MODEMANAGER_INTERFACE_H_

#include "../../StdTypes.h"

#define MODE_MANUAL         0
#define MODE_AUTONOMOUS     1
#define MODE_DIAGNOSTICS    2

void ModeManager_vidInit(void);
void ModeManager_vidRun(void);
void ModeManager_vidSetMode(uint8_t copy_u8Mode);
uint8_t   ModeManager_u8GetMode(void);



#endif /* APP_MODEMANAGER_MODEMANAGER_INTERFACE_H_ */
