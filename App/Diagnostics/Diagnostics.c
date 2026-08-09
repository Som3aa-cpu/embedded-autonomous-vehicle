/*
 * Diagnostics.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */


#include "Diagnostics_interface.h"
#include "Diagnostics_config.h"

#include "../ModeManager/ModeManager_interface.h"
#include "../../HAL/Motor/Motor_interface.h"
#include "../../HAL/HCSR04/HCSR04_interface.h"
#include "../../HAL/IRsensor/IR_interface.h"
#include "../../HAL/LCD/LCD_interface.h"

static uint16_t Diagnostics_u16Tick = 0;

void Diagnostics_vidInit(void) {
    Diagnostics_u16Tick = 0;
    SSD1306_vidInit();
    SSD1306_vidClearBuffer();
    SSD1306_u8UpdateScreen();
}

void Diagnostics_vidRun(void) {
    Diagnostics_u16Tick++;
    if (Diagnostics_u16Tick < DIAGNOSTICS_REFRESH_PERIOD) {
        return;
    }
    Diagnostics_u16Tick = 0;

    uint8_t  u8Mode  = ModeManager_u8GetMode();
    uint16_t u16Dist = HCSR04_u16GetDistanceCm();
    uint8_t  u8Line  = IRSensor_u8ReadRaw();

    SSD1306_vidClearBuffer();

    /* Row 0: Mode */
    switch (u8Mode) {
        case MODE_MANUAL:      SSD1306_vidWriteString(0, 0, "Mode: MANUAL"); break;
        case MODE_AUTONOMOUS:  SSD1306_vidWriteString(0, 0, "Mode: AUTO");   break;
        case MODE_DIAGNOSTICS: SSD1306_vidWriteString(0, 0, "Mode: DIAG");   break;
        default:               SSD1306_vidWriteString(0, 0, "Mode: ???");    break;
    }

    /* Row 1: Distance */
    SSD1306_vidWriteString(0, 1, "Dst: ");
    /* Note: SSD1306_vidWriteString takes const char*; for numbers you'd need itoa helpers.
       For now, just display static labels if no number-to-string helper exists. */
    SSD1306_vidWriteString(40, 1, "cm");

    /* Row 2: IR raw */
    SSD1306_vidWriteString(0, 2, "IR: 0x");

    /* Row 3: Status */
    SSD1306_vidWriteString(0, 3, "System OK");

    SSD1306_u8UpdateScreen();
}
