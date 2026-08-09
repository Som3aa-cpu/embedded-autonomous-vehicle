# PROJECT_CONTEXT.md

# Embedded Autonomous Vehicle Platform
Author: Mohamed Amrallah

---

# Project Overview

This project is an ATmega32-based autonomous ground vehicle developed for an Embedded Systems course.

The goal is to build a modular, layered embedded software architecture while creating a professional portfolio project that demonstrates embedded software engineering practices applicable to automotive and aerospace domains.

The project intentionally avoids Arduino libraries and is written using register-level Embedded C.

---

# Primary Objectives

University Project
- Bluetooth controlled vehicle (via ESP32 BLE co-processor)
- Autonomous line following
- Obstacle detection
- Obstacle avoidance
- Modular software architecture
- Visual status feedback via RGB LEDs

Portfolio Objectives
- Professional GitHub repository
- Clean layered architecture
- Well documented code
- Reusable drivers
- Foundation for future STM32 projects

---

# Development Environment

IDE:
- Eclipse (ATmega32)
- Arduino IDE (ESP32 co-processor firmware)

Language:
- Embedded C (ATmega32)
- C++ / Arduino Framework (ESP32)

Microcontrollers:
- ATmega32 (Main controller — real-time control)
- ESP32-WROOM-32 (Wireless co-processor — BLE bridge)

Programming Method:
- USB Programmer (ATmega32, used during development)
- USB-UART (ESP32, direct flash)

No Arduino framework on ATmega32.

---

# Hardware

## Controller

ATmega32 Development Board

Features:
- On-board 5V regulator
- USB programmer support
- GPIO headers
- Timer/PWM support
- UART
- I2C (TWI)
- ADC

---

## Interface Board

Optional.

Contains:

- LCD
- LEDs
- Push Buttons
- Keypad
- Buzzer
- External EEPROM

The interface board is NOT intended to be mounted on the robot.

It is mainly used for:

- Driver testing
- Debugging
- Demonstrations
- Learning peripherals

The robot itself should operate without the interface board.

---

## Robot Platform

4WD Acrylic Robot Chassis

Includes:

- 4 DC Geared Motors
- 4 Wheels
- Acrylic Chassis
- Mounting Hardware

Drive Method:

Differential Drive

Left Front + Left Rear

↓

Motor Driver Channel A

Right Front + Right Rear

↓

Motor Driver Channel B

Only one L298N motor driver is required.

---

## Purchased Components

Motor Driver

- L298N

Communication

- ~~HC-05 Bluetooth Module~~ → **ESP32-WROOM-32 DevKit** (BLE co-processor)

Obstacle Detection

- HC-SR04 Ultrasonic Sensor
- HC-SR04 Mount

Power

- 2 × 18650 Batteries
- 2x18650 Battery Holder
- 18650 Charger
- Metal Power Switch

Status Indication

- 3 × LEDs (Red, Green, Blue) + 330Ω resistors

Planned

- 5 Channel IR Line Sensor
- MPU6050 IMU

Added (not in original plan)

- 0.96" 128x64 SSD1306 OLED (I2C, 4-pin) — status/diagnostics display
- AT24C08 Serial EEPROM (I2C, DIP-8) — config/calibration storage, on interface board
- **ESP32-WROOM-32** — BLE wireless co-processor replacing HC-05

---

# ESP32 Co-Processor

Role

The ESP32 is the dedicated wireless communication and user-control interface for the car.

Responsibilities:

📱 Bluetooth Low Energy (BLE) — connects the car to the iPhone via Nordic UART Service (NUS).
🎮 Receives driving commands — Forward, Backward, Left, Right, Stop.
🔄 Mode switching — receives commands to switch between Manual, Autonomous, and Diagnostics.
📡 Communicates with the ATmega32 via UART — sends commands from the phone to the ATmega32 using the AVP-Link framed binary protocol.
📊 Receives telemetry from the ATmega32 — speed, distance, obstacle status, IMU data, and forwards to the phone.

Why BLE over Classic Bluetooth?

- iPhone does not support Bluetooth Classic SPP.
- BLE uses the standard Nordic UART Service, compatible with nRF Connect / LightBlue.
- ESP32 handles all wireless complexity; ATmega32 sees only a UART stream.

Firmware

- Written in Arduino framework (C++).
- Single sketch: initializes BLE NUS, parses phone text commands, converts to AVP-Link binary frames.
- Flashed once via Arduino IDE; no further programming needed during normal use.

---

# Status LEDs

Three onboard LEDs provide immediate visual feedback of system state without needing the OLED or phone.

| LED | Color | Pin | Meaning |
|-----|-------|-----|---------|
| **D1** | Red | PA5 | **Manual Mode** active |
| **D2** | Green | PA6 | **Autonomous Mode** active |
| **D3** | Blue | PA7 | **Obstacle Detected** (in Manual or Auto) |

LED Behavior by Mode:

| Mode | Red | Green | Blue | Notes |
|------|-----|-------|------|-------|
| **Manual** | **ON** | OFF | ON if obstacle < threshold | Driver controls via phone |
| **Autonomous** | OFF | **ON** | ON if obstacle < threshold | Line follower active |
| **Diagnostics** | **ON** | **ON** | **ON** | All LEDs lit; OLED shows sensor data |
| **Stop / Idle** | OFF | OFF | OFF | No active mode |

Wiring: LED anode → PA5/PA6/PA7. LED cathode → 330Ω resistor → GND. Active-high logic.

---

# MPU6050

Purpose

Current Project

- Motion monitoring
- Tilt detection
- Robot orientation

Future Projects

- Flight controller
- Artificial Horizon
- Automotive data logger
- Sensor Fusion
- STM32 portfolio projects

Communication

I2C

Pins

VCC
GND
SDA
SCL

INT optional.

---

# EEPROM

Current Status

Not required.

Future Usage

- Save settings
- Calibration values
- Robot speed
- Last operating mode
- PID constants

The EEPROM exists on the interface board.

---

# Power System

Power Source

2 × 18650 Batteries

Power Flow

Battery

↓

Power Switch

↓

Split

↓

ATmega Board Power Input

↓

On-board 5V Regulator

↓

5V Rail

↓

ESP32 VIN (5V)

HC-SR04

IR Sensor

MPU6050

SSD1306 OLED

Status LEDs (via current-limiting resistors)

Second Branch

↓

L298N VIN

Ground

Battery Ground is COMMON GROUND.

Connected to

- ATmega
- L298N
- ESP32
- HC-SR04
- IR Sensors
- MPU6050
- OLED
- LEDs

Everything shares the same ground reference.

---

# Pin Mapping (Finalized)

PORTA — Analog Sensors + Status LEDs

| Pin | Function | Direction | Connected To |
|-----|----------|-----------|--------------|
| PA0 | IR Sensor Ch 0 | Input | 5-Channel IR Array — Far Right |
| PA1 | IR Sensor Ch 1 | Input | 5-Channel IR Array — Right |
| PA2 | IR Sensor Ch 2 | Input | 5-Channel IR Array — Center |
| PA3 | IR Sensor Ch 3 | Input | 5-Channel IR Array — Left |
| PA4 | IR Sensor Ch 4 | Input | 5-Channel IR Array — Far Left |
| PA5 | LED_RED | Output | Red LED (Manual Mode) — 330Ω to GND |
| PA6 | LED_GREEN | Output | Green LED (Auto Mode) — 330Ω to GND |
| PA7 | LED_BLUE | Output | Blue LED (Obstacle) — 330Ω to GND |

PORTB — Ultrasonic + ISP

| Pin | Function | Direction | Connected To |
|-----|----------|-----------|--------------|
| PB0 | FREE | — | Available |
| PB1 | FREE | — | Available |
| PB2 | FREE | — | Available |
| PB3 | FREE | — | Timer0/OC0 free; servo/buzzer |
| PB4 | ULTRASONIC_TRIG | Output | HC-SR04 Trigger |
| PB5 | MOSI | Output | ISP Programmer — Reserved |
| PB6 | MISO | Input | ISP Programmer — Reserved |
| PB7 | SCK | Output | ISP Programmer — Reserved |

PORTC — I2C Bus + Motor Direction

| Pin | Function | Direction | Connected To |
|-----|----------|-----------|--------------|
| PC0 | SCL | I/O | MPU6050, SSD1306, AT24C08 — Hardware TWI, fixed |
| PC1 | SDA | I/O | MPU6050, SSD1306, AT24C08 — Hardware TWI, fixed |
| PC2 | FREE | — | Available |
| PC3 | FREE | — | Available |
| PC4 | MOTOR_L_IN1 | Output | L298N IN1 — Left motor direction |
| PC5 | MOTOR_L_IN2 | Output | L298N IN2 — Left motor direction |
| PC6 | MOTOR_R_IN1 | Output | L298N IN3 — Right motor direction |
| PC7 | MOTOR_R_IN2 | Output | L298N IN4 — Right motor direction |

PORTD — UART + PWM + Ultrasonic Echo

| Pin | Function | Direction | Connected To |
|-----|----------|-----------|--------------|
| PD0 | RXD | Input | ESP32 GPIO17 (TX) — 3.3V → 5V, safe |
| PD1 | TXD | Output | ESP32 GPIO16 (RX) — **5V → 3.3V LEVEL SHIFT REQUIRED** |
| PD2 | FREE / INT0 | — | External interrupt available |
| PD3 | FREE / INT1 | — | External interrupt available |
| PD4 | OC1B | Output | L298N ENA — Left motor PWM |
| PD5 | OC1A | Output | L298N ENB — Right motor PWM |
| PD6 | ICP1 | Input | HC-SR04 Echo — Timer1 Input Capture |
| PD7 | FREE | — | Available |

ESP32 Pin Assignment

| ESP32 Pin | Function | Direction | Connected To |
|-----------|----------|-----------|--------------|
| GPIO16 (RX2) | UART RX | Input | ATmega32 PD1 (TXD) — **Level shifted to 3.3V** |
| GPIO17 (TX2) | UART TX | Output | ATmega32 PD0 (RXD) — 3.3V → 5V safe |
| 5V (VIN) | Power | Input | ATmega32 5V Rail |
| GND | Ground | — | Common GND with ATmega32 |

Notes

- Motor direction pins were moved off PC0/PC1 to keep the hardware TWI pins clear for the MPU6050.
- ~~HC-05 logic is 3.3V; confirm the module's RXD tolerance for 5V input from the ATmega TXD line, add a level shifter if needed.~~ → **Replaced by ESP32. Level shifter REQUIRED on ATmega32 TXD → ESP32 RX line (5V → 3.3V).**
- Ultrasonic Echo uses ICP1 so the Timer ICU module measures pulse width in hardware instead of software polling.
- Real MCU clock is 11.0592MHz, NOT 8MHz (discovered via timed-blink test after both the OLED and EEPROM failed to ACK on the I2C bus). F_CPU must be set to 11059200UL project-wide (compiler symbol), with no conflicting local #define left in any MCAL config header.
- I2C bus addresses in use: MPU6050 = 0x68 (AD0 tied to GND), SSD1306 OLED = 0x3C, AT24C08 EEPROM = 0x50 (A2 tied to GND, WP tied to GND to allow writes).
- Both motors are driven by Timer1 (OC1A = right, OC1B = left), Fast PWM mode 14, TOP=ICR1=19999, prescaler 8, shared frequency (~69Hz at 11.0592MHz). Timer0/OC0 (PB3) is no longer used by the Motor driver and is free for other purposes.
- Motor_prog.c's MOTOR_FORWARD/MOTOR_BACKWARD IN1/IN2 logic is swapped from the "obvious" polarity to match actual hardware wiring - do not "fix" this back without retesting on hardware.
- **ESP32 UART baud rate: 115200, 8-N-1.** AVP-Link framed binary protocol used between ESP32 and ATmega32.
- **Status LEDs:** PA5 (Red/Manual), PA6 (Green/Auto), PA7 (Blue/Obstacle). Active-high. 330Ω cathode resistors.

---

# Software Architecture

Application

↓

HAL

↓

MCAL

↓

ATmega32

**Plus ESP32 Co-Processor (separate firmware):**

Phone (BLE)

↓

ESP32 (Nordic UART Service + AVP-Link Protocol Engine)

↓ UART 115200

ATmega32

---

# Folder Structure

Software/

APP/

VehicleControl/

LineFollower/

ObstacleAvoidance/

Diagnostics/

ModeManager/

HAL/

~~Bluetooth/~~ → **ESP32CoPro/**

Motor/

Ultrasonic/

IRSensor/

LCD/

Keypad/

EEPROM/

MPU6050/

SSD1306_OLED/

MCAL/

DIO/

ADC/

UART/

TIMERS/

I2C/

INTR/

Bit_math.h

StdTypes.h

PWM functionality belongs inside TIMERS.

ESP32_Firmware/ (Arduino IDE project, outside Eclipse workspace)

ESP32_BLE_Bridge.ino

---

# Driver Responsibilities

MCAL

DIO

GPIO Control

UART

~~Bluetooth~~ → **ESP32 Co-Processor Communication** (AVP-Link)

TIMERS

PWM
Delay
Ultrasonic timing

ADC

Analog Sensors

I2C

MPU6050
EEPROM
SSD1306 OLED

INTR

Interrupt Management

HAL

~~Bluetooth~~ → **ESP32CoPro**

AVP-Link frame parser
Command dispatch
Telemetry packing

Motor

Drive motors using L298N

Ultrasonic

Distance Measurement

IRSensor

Line Detection

MPU6050

Motion Data

LCD

Display (Interface Board)

Keypad

User Input (Interface Board)

EEPROM

Read / Write Settings

SSD1306

On-robot OLED diagnostics display

---

# Application Modules

VehicleControl

Main application orchestrator

ModeManager

Switches between:

Manual

Autonomous

Diagnostics

Updates status LEDs (Red/Green/Blue)

LineFollower

Processes IR Sensors

Drives motors based on line position

ObstacleAvoidance

Processes Ultrasonic Sensor

Executes backup-turn-resume maneuver

Diagnostics

System Information

Refreshes SSD1306 OLED with live sensor data

---

# Communication

UART

~~HC05~~ → **ESP32** (BLE bridge to phone)

I2C

MPU6050
SSD1306 OLED
AT24C08 EEPROM

PWM

Motor Speed

GPIO

Motor Direction
IR Sensors
Ultrasonic Trigger
Status LEDs

BLE (Phone ↔ ESP32)

Nordic UART Service (NUS)
UUID: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E

---

# AVP-Link Protocol (ATmega32 ↔ ESP32)

Lightweight framed binary protocol over UART.

Frame Format

| Byte | Field | Value |
|------|-------|-------|
| 0 | SOF | 0xAA |
| 1 | CMD | Command ID |
| 2 | LEN | Payload length (0–32) |
| 3..N | PAYLOAD | LEN bytes |
| N+1 | CRC8 | XOR of CMD + LEN + PAYLOAD |
| N+2 | EOF | 0x55 |

Command IDs (ESP32 → ATmega32)

| ID | Name | Payload | Description |
|----|------|---------|-------------|
| 0x01 | CMD_DRIVE | [dir, speed] | dir: 'F','B','L','R','S'; speed: 0–100 |
| 0x02 | CMD_MODE | [mode] | 0=Manual, 1=Autonomous, 2=Diagnostics |
| 0x03 | CMD_CONFIG | [id, val] | Reserved for PID/speed calibration |
| 0x04 | CMD_PING | [] | Heartbeat / link test |

Command IDs (ATmega32 → ESP32)

| ID | Name | Payload | Description |
|----|------|---------|-------------|
| 0x81 | CMD_TELEMETRY | Packed sensor data | Distance, line state, speed, IMU |
| 0x82 | CMD_STATUS | [mode, errors] | System mode and error flags |
| 0x83 | CMD_DEBUG | [len, ascii...] | Debug string |

Phone Commands (via nRF Connect / LightBlue)

| Text | Action |
|------|--------|
| F | Forward @ 80% |
| F,100 | Forward @ 100% |
| B / L / R / S | Backward / Left / Right / Stop |
| M,0 | Manual mode |
| M,1 | Autonomous mode |
| M,2 | Diagnostics mode |
| PING | Test link |

---

# Robot Modes

Manual

Controlled by Bluetooth (iPhone → ESP32 → ATmega32)

Commands

F

B

L

R

S

Red LED ON

Autonomous

Line Following

↓

Obstacle?

↓

Avoid

↓

Continue

Green LED ON

Blue LED ON when obstacle detected

Diagnostics

Display status information on SSD1306 OLED

All three LEDs ON (Red + Green + Blue)

---

# Wiring Philosophy

Battery powers

ATmega Board

L298N

ATmega Board provides regulated 5V to

ESP32

HC-SR04

IR Sensor

MPU6050

SSD1306 OLED

Status LEDs (via current-limiting resistors)

All devices share the same Battery Ground.

---

# Coding Philosophy

Never access registers directly from APP.

Flow

APP

↓

HAL

↓

MCAL

↓

Registers

Drivers must remain reusable.

Avoid large main() functions.

Use modules.

---

# GitHub Goals

Professional repository.

Include

README

Architecture

Hardware

Software

State Machine

Pin Mapping

Wiring

Pictures

Videos

Development Log

Datasheets

Frequent commits.

---

# Future Roadmap

Phase 1

ATmega32 Project

Phase 2

STM32 Blue Pill

Reuse

Robot

Motors

Sensors

~~Bluetooth~~ → Wireless module

Battery

L298N

Add

CAN

FreeRTOS

Advanced IMU

Automotive ECU Projects

Flight Controller Projects

---

# Current Status

Completed

✓ Architecture designed

✓ Folder hierarchy completed

✓ Hardware selected

✓ Power system designed

✓ Software layers defined

✓ Pin assignment finalized (see Pin Mapping section)

✓ HAL Motor driver (Motor_interface.h / Motor_config.h / Motor_program.c) — direction + PWM speed control per motor group

✓ HAL IRSensor driver (IRSensor_interface.h / IRSensor_config.h / IRSensor_program.c) — per-channel and raw 5-bit read

✓ HAL MPU6050 driver (MPU6050_interface.h / MPU6050_private.h / MPU6050_config.h / MPU6050_program.c) — blocking I2C init, WHO_AM_I connection check, burst raw read (accel/temp/gyro), g / deg-per-sec / Celsius conversion helpers

✓ HAL OLED driver for SSD1306 128x64 (SSD1306_interface.h / SSD1306_private.h / SSD1306_config.h / SSD1306_program.c / OLED_Font5x7.h) — page-addressed framebuffer, pixel/text drawing, 5x7 font stored in flash (manual PROGMEM, no avr/pgmspace.h due to a broken WinAVR toolchain header) — CONFIRMED WORKING ON HARDWARE

✓ HAL EEPROM driver for AT24C08 (EEPROM_interface.h / EEPROM_private.h / EEPROM_config.h / EEPROM_program.c) — byte/page/buffer read-write, ACK-polling for write-cycle completion, automatic page and 256-byte block boundary splitting — driver complete, hardware test pending

✓ Root-caused and fixed a project-wide clock bug: real MCU clock is 11.0592MHz, project was built assuming 8MHz. This broke I2C timing (TWBR) badly enough that no I2C device could ACK. Fixed via project-wide F_CPU compiler symbol correction.

✓ Root-caused and fixed a bus-lockup bug present in all three I2C device drivers (OLED, MPU6050, EEPROM): early-return failure paths were skipping I2C_vidStopCond(), leaving the TWI bus stuck after any single failed step and cascading into total failure for the rest of an init/transaction sequence. Fixed across all three drivers.

✓ SSD1306 OLED driver — CONFIRMED WORKING ON HARDWARE (text + pixel drawing verified).

✓ Motor driver reworked and CONFIRMED WORKING ON HARDWARE, both channels:
  - Original design split PWM across Timer0 (left) + Timer1 (right); reworked so both motors run off Timer1 (OC1A=right, OC1B=left) for matched frequency/resolution.
  - Found and fixed: Timer1Int() was never being called by Motor_Init() (only FastPWNinit()/Timer0 was) - right motor's PWM was completely unconfigured, motor was totally dead.
  - Found and fixed: Timer1's actual PWM range is 0-19999 (ICR1 TOP), not 0-255 like Timer0 - duty scaling was wrong for any Timer1-driven channel.
  - MCAL Timer_prog.c required two additions to support this: COM1B1:0 bits added to Timer1Int() (previously only enabled OC1A output, not OC1B), and a new Timer1B_CompareMatchValue() function. MCAL Timer_Reg.h required new Timer_OCR1B/Timer_OCR1BH/Timer_OCR1BL macros (0x48/0x49) - these were missing entirely.
  - Found and fixed: MOTOR_FORWARD/MOTOR_BACKWARD IN1/IN2 logic was inverted relative to actual hardware wiring - swapped in Motor_program.c.
  - Found (hardware wiring issue, not code): one of the two parallel left motors had its leads swapped, causing left-front and left-back to spin opposite directions - user resolved by re-wiring, not a driver fix.

✓ **ESP32 BLE Co-Processor firmware designed, flashed, and verified working**
  - Replaced HC-05 with ESP32-WROOM-32 for iPhone compatibility (BLE Nordic UART Service)
  - ESP32 firmware: BLE NUS bridge, text-to-binary AVP-Link protocol translation
  - Verified phone connection via nRF Connect / LightBlue
  - Commands F/B/L/R/S, M,0/1/2, PING all confirmed working

✓ **Status LED system defined and integrated**
  - PA5 = Red (Manual), PA6 = Green (Auto), PA7 = Blue (Obstacle)
  - LED logic integrated into ModeManager and VehicleControl
  - Active-high, 330Ω cathode resistors

✓ **HAL ESP32CoPro driver (ESP32CoPro_interface.h / ESP32CoPro_private.h / ESP32CoPro_config.h / ESP32CoPro_program.c)**
  - Non-blocking AVP-Link frame parser using existing UART HAL
  - Command dispatch (DRIVE, MODE, PING, CONFIG)
  - Telemetry packing and transmission
  - Status and debug string transmission

✓ **APP ModeManager (ModeManager_interface.h / ModeManager_config.h / ModeManager_program.c)**
  - Mode switching with LED feedback
  - Delegates to LineFollower, ObstacleAvoidance, Diagnostics

✓ **APP LineFollower (LineFollower_interface.h / LineFollower_config.h / LineFollower_program.c)**
  - 5-bit IR sensor state machine
  - Straight / gentle turn / sharp turn / lost line / cross line handling
  - Delegates to ObstacleAvoidance when obstacle detected

✓ **APP ObstacleAvoidance (ObstacleAvoidance_interface.h / ObstacleAvoidance_config.h / ObstacleAvoidance_program.c)**
  - State machine: STOP → BACKUP → TURN_AWAY → FORWARD → TURN_BACK → DONE
  - Configurable durations and speeds
  - Reset on mode switch

✓ **APP Diagnostics (Diagnostics_interface.h / Diagnostics_config.h / Diagnostics_program.c)**
  - SSD1306 OLED live readout: mode, speed, distance, IR raw, status
  - Periodic refresh

✓ **APP VehicleControl (VehicleControl_interface.h / VehicleControl_config.h / VehicleControl_program.c)**
  - Clean main() — init + infinite run loop only
  - Command parsing, mode execution, telemetry, obstacle LED

In Progress

- GitHub setup
- HAL Ultrasonic driver (config done, pending implementation)
- MPU6050 hardware bring-up: I2C ACK not yet confirmed with the corrected clock/stop-condition fixes - next step is running the I2C scanner with OLED+MPU6050 both connected to check device count, then confirming WHO_AM_I register value matches (some GY-521 breakout boards use MPU6500/9250 clones with a different WHO_AM_I than 0x68)
- EEPROM hardware bring-up not yet retested since the clock/stop-condition fixes (last hardware attempt showed a false-ACK/bus-fault symptom that predates both fixes)
- Physical wiring of ESP32 to ATmega32 (level shifter, common GND)
- Status LED hardware wiring (PA5-PA7 to LEDs with resistors)

Pending

- Hardware assembly (ESP32 mounting, LED wiring)
- Driver implementation (Ultrasonic — config done, needs logic)
- Integration testing (end-to-end: phone → ESP32 → ATmega32 → motors/sensors/LEDs)
- Full system test on track

---

# Important Notes

This project should resemble a simplified automotive embedded system rather than a toy robot.

The focus is:

- Embedded software quality
- Modularity
- Documentation
- Reusability
- Portfolio quality

The code should always prioritize architecture over quick solutions.
