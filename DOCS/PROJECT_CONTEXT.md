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
- Bluetooth controlled vehicle
- Autonomous line following
- Obstacle detection
- Obstacle avoidance
- Modular software architecture

Portfolio Objectives
- Professional GitHub repository
- Clean layered architecture
- Well documented code
- Reusable drivers
- Foundation for future STM32 projects

---

# Development Environment

IDE:
- Eclipse

Language:
- Embedded C

Microcontroller:
- ATmega32

Programming Method:
- USB Programmer (used during development)

No Arduino framework.

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

# Robot Platform

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

# Purchased Components

Motor Driver

- L298N

Communication

- HC-05 Bluetooth Module

Obstacle Detection

- HC-SR04 Ultrasonic Sensor
- HC-SR04 Mount

Power

- 2 × 18650 Batteries
- 2x18650 Battery Holder
- 18650 Charger
- Metal Power Switch

Planned

- 5 Channel IR Line Sensor
- MPU6050 IMU

Added (not in original plan)

- 0.96" 128x64 SSD1306 OLED (I2C, 4-pin) - status/diagnostics display
- AT24C08 Serial EEPROM (I2C, DIP-8) - config/calibration storage, on interface board

---

# Planned Features

Manual Driving

- Bluetooth Control
- Forward
- Backward
- Left
- Right
- Stop

Autonomous Driving

- Line Following
- Obstacle Detection
- Obstacle Avoidance

Diagnostics

- Bluetooth Status
- Sensor Status
- Error Reporting

Future

- IMU Motion Monitoring
- Event Logging
- EEPROM Configuration Storage

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

HC05

HC-SR04

IR Sensor

MPU6050

Second Branch

↓

L298N VIN

Ground

Battery Ground is COMMON GROUND.

Connected to

- ATmega
- L298N
- HC05
- HC-SR04
- IR Sensors
- MPU6050

Everything shares the same ground reference.

---

# Pin Mapping (Finalized)

PORTA

- PA0–PA4: 5-channel IR line sensor
- PA5–PA7: Free

PORTB

- PB4: Ultrasonic Trigger
- PB0–PB3: Free
- PB5 (MOSI), PB6 (MISO), PB7 (SCK): Reserved for ISP programmer

PORTC

- PC0 (SCL), PC1 (SDA): Hardware I2C — MPU6050, SSD1306 OLED, AT24C08 EEPROM (all share the same bus, different addresses; fixed pins, do not reassign)
- PC4: Left motor direction IN1
- PC5: Left motor direction IN2
- PC6: Right motor direction IN1
- PC7: Right motor direction IN2
- PC2, PC3: Free

PORTD

- PD0 (RXD): HC-05 Bluetooth TXD → ATmega RXD
- PD1 (TXD): ATmega TXD → HC-05 Bluetooth RXD
- PD4 (OC1B): Left motor PWM (ENA)
- PD5 (OC1A): Right motor PWM (ENB)
- PD6 (ICP1): Ultrasonic Echo (Timer1 Input Capture)
- PD2 (INT0), PD3 (INT1), PD7: Free

Notes

- Motor direction pins were moved off PC0/PC1 to keep the hardware TWI pins clear for the MPU6050.
- HC-05 logic is 3.3V; confirm the module's RXD tolerance for 5V input from the ATmega TXD line, add a level shifter if needed.
- Ultrasonic Echo uses ICP1 so the Timer ICU module measures pulse width in hardware instead of software polling.
- Real MCU clock is 11.0592MHz, NOT 8MHz (discovered via timed-blink test after both the OLED and EEPROM failed to ACK on the I2C bus). F_CPU must be set to 11059200UL project-wide (compiler symbol), with no conflicting local #define left in any MCAL config header.
- I2C bus addresses in use: MPU6050 = 0x68 (AD0 tied to GND), SSD1306 OLED = 0x3C, AT24C08 EEPROM = 0x50 (A2 tied to GND, WP tied to GND to allow writes).
- Both motors are driven by Timer1 (OC1A = right, OC1B = left), Fast PWM mode 14, TOP=ICR1=19999, prescaler 8, shared frequency (~69Hz at 11.0592MHz). Timer0/OC0 (PB3) is no longer used by the Motor driver and is free for other purposes.
- Motor_prog.c's MOTOR_FORWARD/MOTOR_BACKWARD IN1/IN2 logic is swapped from the "obvious" polarity to match actual hardware wiring - do not "fix" this back without retesting on hardware.

---

# Software Architecture

Application

↓

HAL

↓

MCAL

↓

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

Bluetooth/

Motor/

Ultrasonic/

IRSensor/

LCD/

Keypad/

EEPROM/

MPU6050/

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

---

# Driver Responsibilities

MCAL

DIO

GPIO Control

UART

Bluetooth Communication

TIMERS

PWM
Delay
Ultrasonic timing

ADC

Analog Sensors

I2C

MPU6050
EEPROM

INTR

Interrupt Management

HAL

Motor

Drive motors using L298N

Bluetooth

HC05 Communication

Ultrasonic

Distance Measurement

IRSensor

Line Detection

MPU6050

Motion Data

LCD

Display

Keypad

User Input

EEPROM

Read / Write Settings

---

# Application Modules

VehicleControl

Main application

ModeManager

Switches between:

Manual

Autonomous

Diagnostics

LineFollower

Processes IR Sensors

ObstacleAvoidance

Processes Ultrasonic Sensor

Diagnostics

System Information

---

# Communication

UART

HC05

I2C

MPU6050

Future EEPROM

PWM

Motor Speed

GPIO

Motor Direction
IR Sensors
Ultrasonic Trigger

---

# Robot Modes

Manual

Controlled by Bluetooth

Commands

F

B

L

R

S

Autonomous

Line Following

↓

Obstacle?

↓

Avoid

↓

Continue

Diagnostics

Display status information

---

# Wiring Philosophy

Battery powers

ATmega Board

L298N

ATmega Board provides regulated 5V to

HC05

HC-SR04

IR Sensor

MPU6050

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

Bluetooth

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

In Progress

- GitHub setup
- HAL Ultrasonic driver (config done, pending implementation)
- HAL Bluetooth driver (config done, pending implementation)
- MPU6050 hardware bring-up: I2C ACK not yet confirmed with the corrected clock/stop-condition fixes - next step is running the I2C scanner with OLED+MPU6050 both connected to check device count, then confirming WHO_AM_I register value matches (some GY-521 breakout boards use MPU6500/9250 clones with a different WHO_AM_I than 0x68)
- EEPROM hardware bring-up not yet retested since the clock/stop-condition fixes (last hardware attempt showed a false-ACK/bus-fault symptom that predates both fixes)

Pending

- Hardware assembly
- Driver implementation (Ultrasonic, Bluetooth)
- Integration
- Testing

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