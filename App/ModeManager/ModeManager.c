/*
 * ModeManager.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */

#include "ModeManager_interface.h"
#include "ModeManager_config.h"
#include "../../MCAL/DIO/DIO_Interface.h"


/* LED Pin Definitions (from pin mapping) */
#define LED_RED_PORT    DIO_PORTA
#define LED_RED_PIN     DIO_PIN5

#define LED_GREEN_PORT  DIO_PORTA
#define LED_GREEN_PIN   DIO_PIN6

#define LED_BLUE_PORT   DIO_PORTA
#define LED_BLUE_PIN    DIO_PIN7

static uint8_t ModeManager_u8CurrentMode;

static void ModeManager_vidUpdateLEDs(void);

void ModeManager_vidInit(void) {
    /* Configure LED pins as outputs */
    DIO_setPinDirection(LED_RED_PORT,   LED_RED_PIN,   DIO_PIN_OUTPUT);
    DIO_setPinDirection(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_OUTPUT);

    /* Start in default mode */
    ModeManager_u8CurrentMode = MODEMANAGER_DEFAULT_MODE;
    ModeManager_vidUpdateLEDs();

    LineFollower_vidInit();
    ObstacleAvoidance_vidInit();
    Diagnostics_vidInit();
}

void ModeManager_vidSetMode(uint8_t copy_u8Mode) {
    if (copy_u8Mode <= MODE_DIAGNOSTICS) {
        /* Reset avoidance state machine on any mode switch */
        ObstacleAvoidance_vidReset();
        ModeManager_u8CurrentMode = copy_u8Mode;
        ModeManager_vidUpdateLEDs();
    }
}

uint8_t ModeManager_u8GetMode(void) {
    return ModeManager_u8CurrentMode;
}

void ModeManager_vidRun(void) {
    switch (ModeManager_u8CurrentMode) {
        case MODE_MANUAL:
            /* Manual: VehicleControl handles drive commands from BLE */
            break;

        case MODE_AUTONOMOUS:
            LineFollower_vidRun();
            break;

        case MODE_DIAGNOSTICS:
            Diagnostics_vidRun();
            break;

        default:
            ModeManager_u8CurrentMode = MODE_MANUAL;
            ModeManager_vidUpdateLEDs();
            break;
    }
}

/*
 * LED Logic:
 *   Red   = Manual mode
 *   Green = Autonomous mode
 *   Blue  = Obstacle detected (handled by VehicleControl, not here)
 *   All 3 = Diagnostics mode
 */
static void ModeManager_vidUpdateLEDs(void) {
    /* Clear all LEDs first */
    DIO_setPinValue(LED_RED_PORT,   LED_RED_PIN,   DIO_PIN_LOW);
    DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_LOW);
    DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_LOW);

    switch (ModeManager_u8CurrentMode) {
        case MODE_MANUAL:
            DIO_setPinValue(LED_RED_PORT, LED_RED_PIN, DIO_PIN_HIGH);
            break;

        case MODE_AUTONOMOUS:
            DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_HIGH);
            break;

        case MODE_DIAGNOSTICS:
            DIO_setPinValue(LED_RED_PORT,   LED_RED_PIN,   DIO_PIN_HIGH);
            DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_HIGH);
            DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_HIGH);
            break;
    }
}
