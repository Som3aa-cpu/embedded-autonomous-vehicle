/*
 * ModeManager.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#include "ModeManager_interface.h"
#include "ModeManager_config.h"

static uint8_t ModeManager_u8CurrentMode;

void ModeManager_vidInit(void) {
    ModeManager_u8CurrentMode = MODEMANAGER_DEFAULT_MODE;
}

void ModeManager_vidSetMode(uint8_t copy_u8Mode) {
    if (copy_u8Mode <= MODE_DIAGNOSTICS) {
        ModeManager_u8CurrentMode = copy_u8Mode;
    }
}

uint8_t ModeManager_u8GetMode(void) {
    return ModeManager_u8CurrentMode;
}

void ModeManager_vidRun(void) {
    switch (ModeManager_u8CurrentMode) {
        case MODE_MANUAL:
            /* Manual mode: commands handled directly by VehicleControl */
            break;

        case MODE_AUTONOMOUS:
            /* TODO: Call LineFollower_vidRun() and ObstacleAvoidance_vidRun() */
            break;

        case MODE_DIAGNOSTICS:
            /* TODO: Refresh OLED with sensor readouts */
            break;

        default:
            ModeManager_u8CurrentMode = MODE_MANUAL;
            break;
    }
}
