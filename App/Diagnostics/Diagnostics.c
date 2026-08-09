/*
 * Diagnostics.c
 *
 *  Created on: 9 Aug 2026
 *      Author: mohib
 */


#include "Diagnostics_interface.h"
#include "Diagnostics_config.h"

#include "ModeManager_interface.h"
#include "Motor_interface.h"
#include "../../HAL/HCSR04/HCSR04_interface.h"
#include "../../HAL/IRsensor/IR_interface.h"
#include "../../HAL/LCD/LCD_interface.h"

static uint16_t Diagnostics_uint16_tTick = 0;

/* Private helper: convert uint8_t to decimal string, 3 digits max */
static void Diagnostics_viduint8_tToStr(uint8_t value, uint8_t* buf) {
    buf[0] = (value / 100) + '0';
    buf[1] = ((value % 100) / 10) + '0';
    buf[2] = (value % 10) + '0';
    if (buf[0] == '0') buf[0] = ' ';
    if (buf[0] == ' ' && buf[1] == '0') buf[1] = ' ';
}

/* Private helper: convert uint16_t to decimal string, 5 digits max */
static void Diagnostics_viduint16_tToStr(uint16_t value, uint8_t* buf) {
    buf[0] = (value / 10000) + '0';
    buf[1] = ((value % 10000) / 1000) + '0';
    buf[2] = ((value % 1000) / 100) + '0';
    buf[3] = ((value % 100) / 10) + '0';
    buf[4] = (value % 10) + '0';
    if (buf[0] == '0') buf[0] = ' ';
    if (buf[0] == ' ' && buf[1] == '0') buf[1] = ' ';
    if (buf[0] == ' ' && buf[1] == ' ' && buf[2] == '0') buf[2] = ' ';
}

void Diagnostics_vidInit(void) {
    Diagnostics_uint16_tTick = 0;
    SSD1306_vidClear();
}

void Diagnostics_vidRun(void) {
    Diagnostics_uint16_tTick++;
    if (Diagnostics_uint16_tTick < DIAGNOSTICS_REFRESH_PERIOD) {
        return;
    }
    Diagnostics_uint16_tTick = 0;

    uint8_t  uint8_tMode  = ModeManager_uint8_tGetMode();
    uint8_t  uint8_tSpeed = Motor_uint8_tGetCurrentSpeed();
    uint16_t uint16_tDist = Ultrasonic_uint16_tGetDistance();
    uint8_t  uint8_tLine  = IRSensor_uint8_tGetRawValue();

    uint8_t auint8_tBuf[16];

    SSD1306_vidClear();

    /* Row 0: Mode */
    SSD1306_vidSetCursor(0, 0);
    switch (uint8_tMode) {
        case MODE_MANUAL:      SSD1306_vidDrawString((uint8_t*)"Mode: MANUAL"); break;
        case MODE_AUTONOMOUS:  SSD1306_vidDrawString((uint8_t*)"Mode: AUTO");   break;
        case MODE_DIAGNOSTICS: SSD1306_vidDrawString((uint8_t*)"Mode: DIAG");   break;
        default:               SSD1306_vidDrawString((uint8_t*)"Mode: ???");    break;
    }

    /* Row 1: Speed */
    SSD1306_vidSetCursor(0, 1);
    Diagnostics_viduint8_tToStr(uint8_tSpeed, auint8_tBuf);
    auint8_tBuf[3] = '%';
    auint8_tBuf[4] = '\0';
    SSD1306_vidDrawString((uint8_t*)"Spd: ");
    SSD1306_vidDrawString(auint8_tBuf);

    /* Row 2: Ultrasonic distance */
    SSD1306_vidSetCursor(0, 2);
    Diagnostics_viduint16_tToStr(uint16_tDist, auint8_tBuf);
    auint8_tBuf[5] = '\0';
    SSD1306_vidDrawString((uint8_t*)"Dst: ");
    SSD1306_vidDrawString(auint8_tBuf);
    SSD1306_vidDrawString((uint8_t*)" cm");

    /* Row 3: IR sensor raw value */
    SSD1306_vidSetCursor(0, 3);
    auint8_tBuf[0] = '0'; auint8_tBuf[1] = 'x';
    uint8_t uint8_tNibble = (uint8_tLine >> 4) & 0x0F;
    auint8_tBuf[2] = (uint8_tNibble < 10) ? (uint8_tNibble + '0') : (uint8_tNibble - 10 + 'A');
    uint8_tNibble = uint8_tLine & 0x0F;
    auint8_tBuf[3] = (uint8_tNibble < 10) ? (uint8_tNibble + '0') : (uint8_tNibble - 10 + 'A');
    auint8_tBuf[4] = '\0';
    SSD1306_vidDrawString((uint8_t*)"IR:  ");
    SSD1306_vidDrawString(auint8_tBuf);

    /* Row 4: Status */
    SSD1306_vidSetCursor(0, 4);
    SSD1306_vidDrawString((uint8_t*)"System OK");
}
