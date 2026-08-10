/*
 * Main.c
 *
 *  Created on: 5 Aug 2026
 *      Author: mohib
 */

#include "StdTypes.h"
#include "Bit_math.h"
#include "MCAL/DIO/DIO_interface.h"
#include "MCAL/INTR/INTR_interface.h"
#include "MCAL/INTR/GIE_interface.h"
#include "HAL/ESP32/ESP32_interface.h"
#include "HAL/IRsensor/IR_interface.h"
#include "HAL/HCSR04/HCSR04_interface.h"
#include "HAL/LCD/LCD_interface.h"
#include "MCAL/I2C/I2C_interface.h"
#include "HAL/LCD/LCD_config.h"
#include "HAL/EEPROM/EEPROM_interface.h"
#include "HAL/EEPROM/EEPROM_priv.h"
#include "HAL/MPU6050/MPU6050_interface.h"
#include "HAL/MPU6050/MPU6050_priv.h"
#include "HAL/MOTOR/Motor_interface.h"
#include "MCAL/TIMERS/Timer_interface.h"
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 11059200UL
#endif

/* ---- UNVERIFIED — flip these if line-following behaves backwards ---- */
#define IR_LINE_ACTIVE_HIGH   1   /* 1 = sensor bit is 1 when ON the black line. Flip to 0 if inverted. */
#define IR_BIT0_IS_FAR_LEFT   1   /* 1 = bit0 (far-left per driver) really is physical far-left. Flip to 0 if mirrored. */

#define CMD_BUFFER_SIZE        16
#define DEFAULT_SPEED          50U
#define AUTO_BASE_SPEED        35U
#define AUTO_TURN_SPEED        25U
#define AVOID_SPEED             40U
#define OBSTACLE_STOP_CM        15U
#define TRIGGER_INTERVAL_MS     60U
#define LINE_LOST_TIMEOUT_MS   800U   /* if line stays lost this long, stop instead of blind-searching forever */

/* --- Avoidance maneuver timing --- */
#define AVOID_BACKUP_MS        400U
#define AVOID_TURN_MS          350U
#define AVOID_FORWARD_MS       500U
#define AVOID_TURNBACK_MS      350U

/* --- Status LED pins --- */
#define LED_RED_PORT     DIO_PORTA
#define LED_RED_PIN      DIO_PIN5   /* obstacle detected */
#define LED_GREEN_PORT   DIO_PORTA
#define LED_GREEN_PIN    DIO_PIN6   /* autonomous mode active */
#define LED_BLUE_PORT    DIO_PORTA
#define LED_BLUE_PIN     DIO_PIN7   /* manual mode active */

typedef enum { AVOID_STATE_IDLE = 0, AVOID_STATE_BACKUP, AVOID_STATE_TURN_AWAY,
               AVOID_STATE_FORWARD, AVOID_STATE_TURN_BACK } AvoidState_t;

typedef enum { MODE_MANUAL = 0, MODE_AUTO } RobotMode_t;

static char    g_CmdBuffer[CMD_BUFFER_SIZE];
static uint8_t g_CmdIndex = 0;

static RobotMode_t g_Mode = MODE_AUTO;

static uint8_t g_CurrentSpeed   = DEFAULT_SPEED;
static uint8_t g_ObstacleFlag   = 0;

static AvoidState_t g_AvoidState = AVOID_STATE_IDLE;
static uint16_t      g_AvoidTimer = 0;
static uint8_t        g_TurnRight  = 1;

static sint8_t   g_LastLineDir   = 0;   /* -1 = last seen left, +1 = last seen right, 0 = unknown */
static uint16_t g_LineLostTimer = 0;

/* ---- Low-level drive helpers (FORWARD/BACKWARD swapped per hardware verification) ---- */

static void Drive_Forward(uint8_t speed)
{
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void Drive_Backward(uint8_t speed)
{
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void Drive_PivotLeft(uint8_t speed)
{
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_FORWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

static void Drive_PivotRight(uint8_t speed)
{
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_FORWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  speed);
    Motor_SetSpeed(MOTOR_RIGHT, speed);
}

/* Differential drive for proportional line-follow turns (both wheels forward,
 * one slower rather than a full pivot, so it curves instead of spinning) */
static void Drive_Curve(uint8_t leftSpeed, uint8_t rightSpeed)
{
    Motor_SetDirection(MOTOR_LEFT,  MOTOR_BACKWARD);
    Motor_SetDirection(MOTOR_RIGHT, MOTOR_BACKWARD);
    Motor_SetSpeed(MOTOR_LEFT,  leftSpeed);
    Motor_SetSpeed(MOTOR_RIGHT, rightSpeed);
}

static void Drive_Stop(void)
{
    Motor_StopAll();
}

/* ---- LEDs ---- */

static void LED_Init(void)
{
    DIO_setPinDirection(LED_RED_PORT,   LED_RED_PIN,   DIO_PIN_OUTPUT);
    DIO_setPinDirection(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_OUTPUT);
    DIO_setPinDirection(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_OUTPUT);
    DIO_setPinValue(LED_RED_PORT,   LED_RED_PIN,   DIO_PIN_LOW);
    DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_LOW);
    DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_LOW);
}

static void LED_SetObstacle(uint8_t copy_u8On)
{
    DIO_setPinValue(LED_RED_PORT, LED_RED_PIN, copy_u8On ? DIO_PIN_HIGH : DIO_PIN_LOW);
}

static void LED_UpdateMode(void)
{
    if (g_Mode == MODE_MANUAL)
    {
        DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_HIGH);
        DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_LOW);
    }
    else
    {
        DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_LOW);
        DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_HIGH);
    }
}

/* ---- Command parsing ---- */

static uint8_t ParseSpeedSuffix(const char* copy_pStr)
{
    const char* local_pComma = copy_pStr;
    uint16_t local_u16Val = 0;
    uint8_t local_u8Found = 0;

    while (*local_pComma != '\0')
    {
        if (*local_pComma == ',')
        {
            local_u8Found = 1;
            local_pComma++;
            break;
        }
        local_pComma++;
    }

    if (!local_u8Found) { return DEFAULT_SPEED; }

    while (*local_pComma >= '0' && *local_pComma <= '9')
    {
        local_u16Val = (local_u16Val * 10U) + (uint16_t)(*local_pComma - '0');
        local_pComma++;
    }

    if (local_u16Val > 100U) { local_u16Val = 100U; }

    return (uint8_t)local_u16Val;
}

static void EnterManualMode(void)
{
    g_Mode = MODE_MANUAL;
    g_AvoidState = AVOID_STATE_IDLE;
    Drive_Stop();
    LED_UpdateMode();
}

static void EnterAutoMode(void)
{
    g_Mode = MODE_AUTO;
    g_AvoidState = AVOID_STATE_IDLE;
    g_LineLostTimer = 0;
    Drive_Stop();
    LED_UpdateMode();
}

static void HandleCommand(char* copy_pCmd)
{
    if (copy_pCmd[0] == 'M')
    {
        uint8_t local_u8ModeVal = ParseSpeedSuffix(copy_pCmd);
        if (local_u8ModeVal == 0U) { EnterManualMode(); }
        else if (local_u8ModeVal == 1U) { EnterAutoMode(); }
        g_CmdIndex = 0;
        return;
    }

    if (g_Mode != MODE_MANUAL)
    {
        if (copy_pCmd[0] == 'P')
        {
            USRTSendDataSync((uint16_t)'P');
            USRTSendDataSync((uint16_t)'O');
            USRTSendDataSync((uint16_t)'N');
            USRTSendDataSync((uint16_t)'G');
            USRTSendDataSync((uint16_t)'\n');
        }
        g_CmdIndex = 0;
        return;
    }

    if (copy_pCmd[0] == 'F')
    {
        g_CurrentSpeed = ParseSpeedSuffix(copy_pCmd);
        if (!g_ObstacleFlag) { Drive_Forward(g_CurrentSpeed); }
        else                 { Drive_Stop(); }
    }
    else if (copy_pCmd[0] == 'B')
    {
        g_CurrentSpeed = ParseSpeedSuffix(copy_pCmd);
        Drive_Backward(g_CurrentSpeed);
    }
    else if (copy_pCmd[0] == 'L') { Drive_PivotLeft(DEFAULT_SPEED); }
    else if (copy_pCmd[0] == 'R') { Drive_PivotRight(DEFAULT_SPEED); }
    else if (copy_pCmd[0] == 'S') { Drive_Stop(); }
    else if (copy_pCmd[0] == 'P')
    {
        USRTSendDataSync((uint16_t)'P');
        USRTSendDataSync((uint16_t)'O');
        USRTSendDataSync((uint16_t)'N');
        USRTSendDataSync((uint16_t)'G');
        USRTSendDataSync((uint16_t)'\n');
    }

    g_CmdIndex = 0;
}

static void UART_PollCommands(void)
{
    while (USRTDataAvailable())
    {
        char local_cByte = (char)USRTReadDataReg();

        if (local_cByte == '\r' || local_cByte == '\n')
        {
            if (g_CmdIndex > 0)
            {
                g_CmdBuffer[g_CmdIndex] = '\0';
                HandleCommand(g_CmdBuffer);
            }
        }
        else if (g_CmdIndex < (CMD_BUFFER_SIZE - 1))
        {
            g_CmdBuffer[g_CmdIndex++] = local_cByte;
        }
        else
        {
            g_CmdIndex = 0;
        }
    }
}

/* ---- Obstacle avoidance state machine ---- */

static void Avoid_Start(void)
{
    g_AvoidState = AVOID_STATE_BACKUP;
    g_AvoidTimer = 0;
    Drive_Backward(AVOID_SPEED);
}

static void Avoid_Tick(void)
{
    if (g_AvoidState == AVOID_STATE_IDLE) { return; }

    g_AvoidTimer++;

    switch (g_AvoidState)
    {
        case AVOID_STATE_BACKUP:
            if (g_AvoidTimer >= AVOID_BACKUP_MS)
            {
                g_AvoidTimer = 0;
                g_AvoidState = AVOID_STATE_TURN_AWAY;
                if (g_TurnRight) { Drive_PivotRight(AVOID_SPEED); }
                else             { Drive_PivotLeft(AVOID_SPEED);  }
            }
            break;

        case AVOID_STATE_TURN_AWAY:
            if (g_AvoidTimer >= AVOID_TURN_MS)
            {
                g_AvoidTimer = 0;
                g_AvoidState = AVOID_STATE_FORWARD;
                Drive_Forward(AVOID_SPEED);
            }
            break;

        case AVOID_STATE_FORWARD:
            if (g_AvoidTimer >= AVOID_FORWARD_MS)
            {
                g_AvoidTimer = 0;
                g_AvoidState = AVOID_STATE_TURN_BACK;
                if (g_TurnRight) { Drive_PivotLeft(AVOID_SPEED);  }
                else             { Drive_PivotRight(AVOID_SPEED); }
            }
            break;

        case AVOID_STATE_TURN_BACK:
            if (g_AvoidTimer >= AVOID_TURNBACK_MS)
            {
                g_AvoidTimer = 0;
                g_AvoidState = AVOID_STATE_IDLE;
                g_TurnRight = !g_TurnRight;
            }
            break;

        default:
            g_AvoidState = AVOID_STATE_IDLE;
            break;
    }
}

/* ---- Line following ---- */

/* Normalizes raw bits so bit0 = far-left / bit4 = far-right physically,
 * and 1 = "on the line", regardless of the two toggles above. */
static uint8_t LineFollow_NormalizeRaw(uint8_t rawBits)
{
    uint8_t local_u8Norm = rawBits;

#if (IR_LINE_ACTIVE_HIGH == 0)
    local_u8Norm = (~local_u8Norm) & 0x1F;
#endif

#if (IR_BIT0_IS_FAR_LEFT == 0)
    {
        uint8_t local_u8Rev = 0;
        if (local_u8Norm & 0x01) local_u8Rev |= 0x10;
        if (local_u8Norm & 0x02) local_u8Rev |= 0x08;
        if (local_u8Norm & 0x04) local_u8Rev |= 0x04;
        if (local_u8Norm & 0x08) local_u8Rev |= 0x02;
        if (local_u8Norm & 0x10) local_u8Rev |= 0x01;
        local_u8Norm = local_u8Rev;
    }
#endif

    return local_u8Norm;
}

/* Weighted position: sensors weighted -2,-1,0,+1,+2 (far-left..far-right).
 * Negative = line is to the left of center, positive = to the right. */
static void LineFollow_Run(void)
{
    uint8_t local_u8Raw  = IRSensor_u8ReadRaw();
    uint8_t local_u8Norm = LineFollow_NormalizeRaw(local_u8Raw);

    if (local_u8Norm == 0x00)
    {
        /* Line lost — steer toward the side it was last seen on to reacquire it */
        g_LineLostTimer++;
        if (g_LineLostTimer >= LINE_LOST_TIMEOUT_MS)
        {
            Drive_Stop();
            return;
        }

        if (g_LastLineDir < 0)      { Drive_PivotLeft(AUTO_TURN_SPEED);  }
        else if (g_LastLineDir > 0) { Drive_PivotRight(AUTO_TURN_SPEED); }
        else                        { Drive_Stop(); }
        return;
    }

    g_LineLostTimer = 0;

    /* All 5 on the line at once = intersection/cross line: go straight through */
    if (local_u8Norm == 0x1F)
    {
        Drive_Forward(AUTO_BASE_SPEED);
        return;
    }

    sint16_t local_s16Weighted = 0;
    uint8_t  local_u8Active    = 0;
    const sint8_t local_as8Weights[5] = { -2, -1, 0, 1, 2 };

    for (uint8_t i = 0; i < 5; i++)
    {
        if (local_u8Norm & (1U << i))
        {
            local_s16Weighted += local_as8Weights[i];
            local_u8Active++;
        }
    }

    sint16_t local_s16Error = local_s16Weighted; /* range roughly -2..+2 */
    g_LastLineDir = (local_s16Error < 0) ? -1 : (local_s16Error > 0 ? 1 : 0);

    if (local_s16Error == 0)
    {
        Drive_Forward(AUTO_BASE_SPEED);
        return;
    }

    /* Proportional curve: bigger error -> bigger speed differential */
    uint8_t local_u8Correction = (uint8_t)((local_s16Error < 0 ? -local_s16Error : local_s16Error) * 12U);
    if (local_u8Correction > (AUTO_BASE_SPEED - 10U))
    {
        local_u8Correction = AUTO_BASE_SPEED - 10U; /* keep the slower wheel from going negative */
    }

    if (local_s16Error < 0)
    {
        /* Line is to the left: slow the left wheel so it curves left */
        Drive_Curve((uint8_t)(AUTO_BASE_SPEED - local_u8Correction), AUTO_BASE_SPEED);
    }
    else
    {
        /* Line is to the right: slow the right wheel so it curves right */
        Drive_Curve(AUTO_BASE_SPEED, (uint8_t)(AUTO_BASE_SPEED - local_u8Correction));
    }
}

int main(void)
{
    USRTInit();
    Motor_Init();
    HCSR04_voidInit();
    IRSensor_voidInit();
    LED_Init();
    LED_UpdateMode();
    GIE_VoidEnable();

    uint16_t local_u16TriggerTimer = 0;

    while (1)
    {
        UART_PollCommands();

        local_u16TriggerTimer++;
        if (local_u16TriggerTimer >= TRIGGER_INTERVAL_MS)
        {
            local_u16TriggerTimer = 0;
            HCSR04_voidTriggerPulse();
        }

        if (HCSR04_u8IsReady())
        {
            uint16_t local_u16Dist = HCSR04_u16GetDistanceCm();
            uint8_t local_u8WasObstacle = g_ObstacleFlag;

            g_ObstacleFlag = (local_u16Dist <= OBSTACLE_STOP_CM) ? 1U : 0U;

            if (g_ObstacleFlag != local_u8WasObstacle)
            {
                LED_SetObstacle(g_ObstacleFlag);
            }

            if (g_Mode == MODE_AUTO && g_ObstacleFlag && g_AvoidState == AVOID_STATE_IDLE)
            {
                Avoid_Start();
            }

            if (g_Mode == MODE_MANUAL && g_ObstacleFlag)
            {
                Drive_Stop();
            }
        }

        Avoid_Tick();

        /* Line-follow only runs when: autonomous mode, no obstacle avoidance
         * maneuver in progress, and no obstacle currently blocking */
        if (g_Mode == MODE_AUTO && g_AvoidState == AVOID_STATE_IDLE && !g_ObstacleFlag)
        {
            LineFollow_Run();
        }

        _delay_ms(1);
    }

    return 0;
}
