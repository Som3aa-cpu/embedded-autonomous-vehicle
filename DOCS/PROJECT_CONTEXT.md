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

In Progress

- GitHub setup
- Driver development

Pending

- Hardware assembly
- Pin assignment
- Driver implementation
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