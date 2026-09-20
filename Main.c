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

#define CMD_BUFFER_SIZE        16

/* --- Speeds --- */
#define AUTO_BASE_SPEED        30U
#define AUTO_TURN_SPEED        45U   /* was 35 — faster pivot for curves */
#define AVOID_SPEED             50U
#define OBSTACLE_CRUISE_SPEED  28U

/* --- Ultrasonic zones --- */
#define OBSTACLE_STOP_CM        25U
#define TRIGGER_INTERVAL_MS     60U
#define LINE_LOST_TIMEOUT_MS   150U  /* was 600 — stop quickly if truly lost */

/* --- Avoidance maneuver timing --- */
#define AVOID_BACKUP_MS        500U
#define AVOID_TURN_MS          700U
#define AVOID_FORWARD_MS       900U
#define AVOID_TURNBACK_MS      700U

/* --- LEDs (PORTA) --- */
#define LED_RED_PORT     DIO_PORTA
#define LED_RED_PIN      DIO_PIN5
#define LED_GREEN_PORT   DIO_PORTA
#define LED_GREEN_PIN    DIO_PIN6
#define LED_BLUE_PORT    DIO_PORTA
#define LED_BLUE_PIN     DIO_PIN7

/* --- Buttons (PORTB, active LOW) --- */
#define BTN_LINEFOLLOW_PORT   DIO_PORTB
#define BTN_LINEFOLLOW_PIN    DIO_PIN0
#define BTN_OBSTACLE_PORT     DIO_PORTB
#define BTN_OBSTACLE_PIN      DIO_PIN1
#define BTN_DEBOUNCE_MS       30

/* --- Line-follow PD gains (stronger) --- */
#define LINE_KP   20   /* was 16 */
#define LINE_KD   12   /* was 10 */

/* ------------------------------------------------------------------ */
/*  Types                                                              */
/* ------------------------------------------------------------------ */
typedef enum
{
    MODE_WAITING = 0,
    MODE_LINEFOLLOW,
    MODE_OBSTACLE
} RobotMode_t;

typedef enum
{
    AVOID_STATE_IDLE = 0,
    AVOID_STATE_BACKUP,
    AVOID_STATE_TURN_AWAY,
    AVOID_STATE_FORWARD,
    AVOID_STATE_TURN_BACK
} AvoidState_t;

/* ------------------------------------------------------------------ */
/*  Globals                                                            */
/* ------------------------------------------------------------------ */
static char    g_CmdBuffer[CMD_BUFFER_SIZE];
static uint8_t g_CmdIndex = 0;

static RobotMode_t g_Mode = MODE_WAITING;

static uint8_t g_ObstacleFlag   = 0;

static AvoidState_t g_AvoidState = AVOID_STATE_IDLE;
static uint16_t      g_AvoidTimer = 0;
static uint8_t        g_TurnRight  = 1;

static sint8_t   g_LastLineDir   = 0;
static sint16_t  g_LastLineError = 0;
static uint16_t  g_LineLostTimer = 0;

/* ------------------------------------------------------------------ */
/*  Drive helpers                                                      */
/* ------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------ */
/*  LEDs                                                               */
/* ------------------------------------------------------------------ */
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
    switch (g_Mode)
    {
        case MODE_WAITING:
            DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_LOW);
            DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_LOW);
            break;

        case MODE_LINEFOLLOW:
            DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_LOW);
            DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_HIGH);
            break;

        case MODE_OBSTACLE:
            DIO_setPinValue(LED_BLUE_PORT,  LED_BLUE_PIN,  DIO_PIN_HIGH);
            DIO_setPinValue(LED_GREEN_PORT, LED_GREEN_PIN, DIO_PIN_HIGH);
            break;

        default:
            break;
    }
}

/* ------------------------------------------------------------------ */
/*  Buttons (PORTB, active LOW, internal pull-up, 30 ms debounce)      */
/* ------------------------------------------------------------------ */
typedef struct
{
    uint8_t port;
    uint8_t pin;
    uint8_t counter;
    uint8_t prev;
    uint8_t triggered;
} Button_t;

static Button_t g_BtnLineFollow = { BTN_LINEFOLLOW_PORT, BTN_LINEFOLLOW_PIN, 0, 0, 0 };
static Button_t g_BtnObstacle   = { BTN_OBSTACLE_PORT,   BTN_OBSTACLE_PIN,   0, 0, 0 };

static void Button_Init(void)
{
    DIO_setPinDirection(BTN_LINEFOLLOW_PORT, BTN_LINEFOLLOW_PIN, DIO_PIN_INPUT);
    DIO_setPinDirection(BTN_OBSTACLE_PORT,   BTN_OBSTACLE_PIN,   DIO_PIN_INPUT);

    DIO_activePullUpRessistencePin(BTN_LINEFOLLOW_PORT, BTN_LINEFOLLOW_PIN);
    DIO_activePullUpRessistencePin(BTN_OBSTACLE_PORT,   BTN_OBSTACLE_PIN);
}

static void Button_Update(Button_t* btn)
{
    uint8_t local_u8Val = DIO_PIN_HIGH;
    DIO_getPinValue(btn->port, btn->pin, &local_u8Val);

    uint8_t pressed = (local_u8Val == DIO_PIN_LOW) ? 1 : 0;

    if (pressed)
    {
        if (btn->counter < BTN_DEBOUNCE_MS)
        {
            btn->counter++;
        }
        else if (!btn->prev)
        {
            btn->triggered = 1;
            btn->prev = 1;
        }
    }
    else
    {
        btn->counter = 0;
        btn->prev = 0;
    }
}

static uint8_t Button_IsTriggered(Button_t* btn)
{
    if (btn->triggered)
    {
        btn->triggered = 0;
        return 1;
    }
    return 0;
}

static void Button_PollAll(void)
{
    Button_Update(&g_BtnLineFollow);
    Button_Update(&g_BtnObstacle);

    if (Button_IsTriggered(&g_BtnLineFollow)) { EnterLineFollowMode(); }
    if (Button_IsTriggered(&g_BtnObstacle))   { EnterObstacleMode(); }
}

/* ------------------------------------------------------------------ */
/*  Mode entry helpers                                                 */
/* ------------------------------------------------------------------ */
static void EnterWaitingMode(void)
{
    g_Mode = MODE_WAITING;
    g_AvoidState = AVOID_STATE_IDLE;
    Drive_Stop();
    LED_UpdateMode();
}

 void EnterLineFollowMode(void)
{
    g_Mode = MODE_LINEFOLLOW;
    g_AvoidState = AVOID_STATE_IDLE;
    g_LineLostTimer = 0;
    g_LastLineError = 0;
    Drive_Stop();
    LED_UpdateMode();
}

 void EnterObstacleMode(void)
{
    g_Mode = MODE_OBSTACLE;
    g_AvoidState = AVOID_STATE_IDLE;
    g_TurnRight = 1;
    Drive_Stop();
    LED_UpdateMode();
}

/* ------------------------------------------------------------------ */
/*  UART (kept for wired debug / mode backup: M,1 and M,2)             */
/* ------------------------------------------------------------------ */
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

    if (!local_u8Found) { return 0; }

    while (*local_pComma >= '0' && *local_pComma <= '9')
    {
        local_u16Val = (local_u16Val * 10U) + (uint16_t)(*local_pComma - '0');
        local_pComma++;
    }

    if (local_u16Val > 100U) { local_u16Val = 100U; }

    return (uint8_t)local_u16Val;
}

static void HandleCommand(char* copy_pCmd)
{
    if (copy_pCmd[0] == 'M')
    {
        uint8_t local_u8ModeVal = ParseSpeedSuffix(copy_pCmd);
        if (local_u8ModeVal == 1U)      { EnterLineFollowMode(); }
        else if (local_u8ModeVal == 2U) { EnterObstacleMode(); }
        g_CmdIndex = 0;
        return;
    }

    if (copy_pCmd[0] == 'P')
    {
        USRTSendDataSync((uint16_t)'P');
        USRTSendDataSync((uint16_t)'O');
        USRTSendDataSync((uint16_t)'N');
        USRTSendDataSync((uint16_t)'G');
        USRTSendDataSync((uint16_t)'\n');
        g_CmdIndex = 0;
        return;
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

/* ------------------------------------------------------------------ */
/*  Obstacle avoidance state machine                                   */
/* ------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------ */
/*  Line following  (PD + sharp pivot on outer sensors)                */
/* ------------------------------------------------------------------ */
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

static void LineFollow_Run(void)
{
    uint8_t local_u8Raw  = IRSensor_u8ReadRaw();
    uint8_t local_u8Norm = LineFollow_NormalizeRaw(local_u8Raw);

    /* ---- Lost line: stop quickly if not reacquired ---- */
    if (local_u8Norm == 0x00)
    {
        g_LineLostTimer++;
        if (g_LineLostTimer >= LINE_LOST_TIMEOUT_MS)
        {
            Drive_Stop();
            return;
        }

        /* Spin aggressively toward last known side for 150 ms */
        if (g_LastLineDir < 0)      { Drive_PivotLeft(AUTO_TURN_SPEED);  }
        else if (g_LastLineDir > 0) { Drive_PivotRight(AUTO_TURN_SPEED); }
        else                        { Drive_Stop(); }
        return;
    }

    g_LineLostTimer = 0;

    /* ---- All sensors on line (intersection / wide tape) ---- */
    if (local_u8Norm == 0x1F)
    {
        Drive_Forward(AUTO_BASE_SPEED);
        g_LastLineError = 0;
        return;
    }

    /* ---- Weighted error: -2..+2 per sensor ---- */
    sint16_t local_s16Weighted = 0;
    const sint8_t local_as8Weights[5] = { -2, -1, 0, 1, 2 };

    for (uint8_t i = 0; i < 5; i++)
    {
        if (local_u8Norm & (1U << i))
        {
            local_s16Weighted += local_as8Weights[i];
        }
    }

    sint16_t local_s16Error = local_s16Weighted;
    g_LastLineDir = (local_s16Error < 0) ? -1 : (local_s16Error > 0 ? 1 : 0);

    /* ---- Centered: straight ---- */
    if (local_s16Error == 0)
    {
        Drive_Forward(AUTO_BASE_SPEED);
        g_LastLineError = 0;
        return;
    }

    /* ---- SHARP TURN: line is at the far edge ----
     *  Error <= -2  -> far-left sensor(s) active -> pivot left in place
     *  Error >= +2  -> far-right sensor(s) active -> pivot right in place
     *  This fixes "poor turning on curves".
     * --------------------------------------------------------------- */
    if ((local_s16Error <= -2) || (local_s16Error >= 2))
    {
        if (local_s16Error < 0)
        {
            Drive_PivotLeft(AUTO_TURN_SPEED);
        }
        else
        {
            Drive_PivotRight(AUTO_TURN_SPEED);
        }
        g_LastLineError = local_s16Error;
        return;
    }

    /* ---- GENTLE TURN: PD curve (error = -1 or +1) ---- */
    sint16_t local_s16Derivative = local_s16Error - g_LastLineError;
    g_LastLineError = local_s16Error;

    uint16_t local_u16ErrAbs = (local_s16Error < 0) ? (uint16_t)(-local_s16Error) : (uint16_t)local_s16Error;
    uint16_t local_u16DerAbs = (local_s16Derivative < 0) ? (uint16_t)(-local_s16Derivative) : (uint16_t)local_s16Derivative;

    uint16_t local_u16Correction = (uint16_t)((local_u16ErrAbs * LINE_KP) + (local_u16DerAbs * LINE_KD));

    /* Allow inner wheel to slow all the way to 0 (was BASE-10) */
    if (local_u16Correction > AUTO_BASE_SPEED)
    {
        local_u16Correction = AUTO_BASE_SPEED;
    }

    uint8_t local_u8Corr = (uint8_t)local_u16Correction;

    if (local_s16Error < 0)
    {
        Drive_Curve((uint8_t)(AUTO_BASE_SPEED - local_u8Corr), AUTO_BASE_SPEED);
    }
    else
    {
        Drive_Curve(AUTO_BASE_SPEED, (uint8_t)(AUTO_BASE_SPEED - local_u8Corr));
    }
}

/* ------------------------------------------------------------------ */
/*  Main                                                               */
/* ------------------------------------------------------------------ */
int main(void)
{
    USRTInit();
    Motor_Init();
    HCSR04_voidInit();
    IRSensor_voidInit();
    LED_Init();
    Button_Init();
    LED_UpdateMode();
    GIE_VoidEnable();

    uint16_t local_u16TriggerTimer = 0;

    while (1)
    {
        UART_PollCommands();
        Button_PollAll();

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

            if (local_u16Dist <= OBSTACLE_STOP_CM)
            {
                g_ObstacleFlag = 1U;
            }
            else
            {
                g_ObstacleFlag = 0U;
            }

            if (g_ObstacleFlag != local_u8WasObstacle)
            {
                LED_SetObstacle(g_ObstacleFlag);
            }
        }

        Avoid_Tick();

        switch (g_Mode)
        {
            case MODE_WAITING:
                Drive_Stop();
                break;

            case MODE_LINEFOLLOW:
                if (g_ObstacleFlag)
                {
                    Drive_Stop();
                }
                else
                {
                    LineFollow_Run();
                }
                break;

            case MODE_OBSTACLE:
                if (g_AvoidState == AVOID_STATE_IDLE)
                {
                    if (g_ObstacleFlag)
                    {
                        Avoid_Start();
                    }
                    else
                    {
                        Drive_Forward(OBSTACLE_CRUISE_SPEED);
                    }
                }
                break;

            default:
                EnterWaitingMode();
                break;
        }

        _delay_ms(1);
    }

    return 0;
}
