


Wednesday 3:25 PM

AVR Graduation Project .pdf
PDF
hey these are the projects iption i have. I pick one of them or think about my own

You clearly miss understood me or I didn’t explain it right so her the photo of both the MC and interface kit 

image(1).png
if you ask about PWN it is in timers 

This is actually a very solid structure, especially for a university project. I like that you're already separating HAL from MCAL.

A few suggestions to make it look even more like a professional embedded project.

Overall Structure
Includes
│
├── HAL
│   ├── Bluetooth
│   ├── EEPROM
│   ├── IRSensor
│   ├── Keypad
│   ├── LCD
│   ├── Motor
│   └── Ultrasonic
│
├── MCAL
│   ├── ADC_Driver
│   ├── DIO
│   ├── I2C
│   ├── INTR
│   ├── TIMERS
│   └── UART
│
├── Bit_math.h
└── StdTypes.h
I'd rate this 8.5/10 already.

Things I'd add
1. APP Folder ⭐⭐⭐⭐⭐
Right now you have the drivers, but where does the robot's intelligence live?

I'd add:

APP
│
├── VehicleControl
├── LineFollower
├── ObstacleAvoidance
├── Diagnostics
└── ModeManager
This keeps main.c very clean.

Instead of

while(1)
{
   ...
   ...
   ...
   700 lines
}
you'll have

while(1)
{
    APP_Run();
}
2. Config Folder
Later you'll have lots of values like

#define MOTOR_SPEED 80

#define SAFE_DISTANCE 20

#define BLUETOOTH_BAUDRATE 9600
Instead of scattering them everywhere

Create

Config

RobotConfig.h
3. Services Layer (Optional)
Professional projects sometimes have:

Services

Scheduler

EEPROM Manager

Logger
For a university project this is optional.

MCAL
Looks good.

I'd maybe rename

ADC_Driver
to

ADC
Every folder is a driver anyway.

HAL
Looks good too.

One thing:

Instead of

Bluetooth
I'd call it

HC05
because the UART driver is already handling Bluetooth communication underneath.

Similarly,

Ultrasonic
could become

HCSR04
This makes it obvious which hardware you're supporting.

Folder Example
Eventually I'd expect something like

HAL

LCD
    LCD.c
    LCD.h
    LCD_cfg.h

Motor
    Motor.c
    Motor.h
    Motor_cfg.h

HC05
    HC05.c
    HC05.h
    HC05_cfg.h
EEPROM
One question:

Is this

EEPROM
the external EEPROM on your interface board

or

the internal EEPROM inside the ATmega32?

If it's the internal EEPROM, I'd actually move it to MCAL, because it's part of the microcontroller itself.

If it's the external EEPROM chip on the interface board (connected via I²C or SPI), then HAL is the right place.

PWM
if you ask about PWM it is in timers

That's exactly where I would put it.

I'd simply expose functions like

PWM_Init();

PWM_SetDutyCycle();

PWM_Start();

PWM_Stop();
inside the timer driver.

No need for another folder.

My only major addition
I would definitely add

APP
because that's what separates

"I wrote some drivers"

from

"I built an embedded application."

The architecture becomes

Application

↓

HAL

↓

MCAL

↓

ATmega32
which is exactly how many embedded projects are organized.

Overall I'd give this a 9.5/10 once you add the APP layer. It's clean, modular, and will make your GitHub repository look much more professional than having everything in one folder.


first the app have folders inside or gonna be code file, and the EEPROM is on the interface kit not in thr MC

Perfect, then EEPROM belongs in HAL because it's an external device on the interface board. I wouldn't move it.

Now about the APP folder.

Option 1 (Recommended)
I would make it folders, not just a single file.

APP/
│
├── VehicleControl/
│   ├── VehicleControl.c
│   └── VehicleControl.h
│
├── LineFollower/
│   ├── LineFollower.c
│   └── LineFollower.h
│
├── ObstacleAvoidance/
│   ├── ObstacleAvoidance.c
│   └── ObstacleAvoidance.h
│
├── ModeManager/
│   ├── ModeManager.c
│   └── ModeManager.h
│
└── Diagnostics/
    ├── Diagnostics.c
    └── Diagnostics.h
This scales really well if the project grows.

Option 2 (Simpler)
APP/
│
├── APP.c
├── APP.h
├── StateMachine.c
├── StateMachine.h
This is perfectly acceptable for a smaller university project.

Which one would I choose?
Considering:

This is your first serious embedded project.

You're putting it on GitHub.

You want it to help with automotive/aerospace opportunities.

I'd choose Option 1.

It looks much more like an industrial embedded project.

How everything connects
main.c
    │
    ▼
VehicleControl
    │
    ├──────────────┐
    ▼              ▼
LineFollower   ObstacleAvoidance
    │              │
    └──────┬───────┘
           ▼
ModeManager
           │
           ▼
HAL Drivers
           │
           ▼
MCAL Drivers
           │
           ▼
ATmega32
Notice something important:

The APP never directly manipulates registers.

For example:

VehicleControl
        │
        ▼
Motor_Forward(80);
The Motor driver handles the details.

The Motor driver calls:

PWM_SetDutyCycle()

DIO_WritePin()
The Timer and DIO drivers then manipulate the registers.

That's proper layering.

I have one more suggestion
Instead of calling it:

APP
consider:

Application
or

APP
Both are common, but APP is shorter and you'll see it a lot in embedded projects.

Final architecture
Includes/
│
├── APP
│   ├── VehicleControl
│   ├── LineFollower
│   ├── ObstacleAvoidance
│   ├── Diagnostics
│   └── ModeManager
│
├── HAL
│   ├── Bluetooth (or HC05)
│   ├── EEPROM
│   ├── IRSensor
│   ├── Keypad
│   ├── LCD
│   ├── Motor
│   └── Ultrasonic (or HCSR04)
│
├── MCAL
│   ├── ADC
│   ├── DIO
│   ├── I2C
│   ├── INTR
│   ├── TIMERS
│   └── UART
│
├── Bit_math.h
└── StdTypes.h
I'd genuinely be happy to see this structure in a GitHub repository from a student. It's clean, modular, and shows that you've thought about software architecture instead of just writing code until it works. It also leaves plenty of room to add features without turning main.c into a giant file.


can you give me file .md to sum up what we did here so I can use it in new chats to keep you to speed or to use other AI agents

That's actually a great idea. Keeping a project summary lets you continue in new chats without losing context and makes it easy to collaborate with other AI tools.

Here's a Markdown file you can save as PROJECT_CONTEXT.md.

# Embedded Autonomous Vehicle Platform
**Author:** Mohamed Amrallah

---

# Project Overview

This project is an ATmega32-based autonomous vehicle developed as an embedded systems course project.

The goal is to build a modular embedded software architecture while implementing:

- Bluetooth manual control
- Autonomous line following
- Obstacle detection
- Multiple operating modes
- LCD diagnostics
- EEPROM event logging

The project is intended to resemble a simplified embedded automotive ECU rather than a basic RC car.

---

# Development Goals

## Primary Goal

Complete the university project within two weeks.

## Secondary Goal

Create a professional GitHub repository that can later be shown to employers and master's admissions.

The project should follow good embedded software architecture and be reusable as a portfolio project.

---

# Hardware

## Microcontroller

- ATmega32
- Development kit provided by the institution

## Interface Board (Provided)

Contains:

- LCD
- LEDs
- Push Buttons
- Keypad
- Buzzer
- External EEPROM

The interface board connects to the ATmega development board using jumper wires.

---

# External Components

## Required

- 2WD Robot Chassis
- 2 DC Geared Motors
- Motor Driver (L298N preferred if already available)
- HC-05 Bluetooth Module
- HC-SR04 Ultrasonic Sensor
- 5-Channel IR Line Sensor Array
- 2×18650 Batteries
- Battery Holder
- Battery Charger
- Jumper Wires
- Power Switch

## Optional

- MPU6050 IMU
- Servo Motor
- Wheel Encoders

These optional components are mainly intended for portfolio expansion.

---

# Operating Modes

## Manual Mode

Controlled using Bluetooth.

Commands:

F = Forward

B = Backward

L = Left

R = Right

S = Stop

---

## Autonomous Mode

Vehicle performs:

- Line Following
- Obstacle Detection
- Obstacle Avoidance

---

## Diagnostics Mode

Displays information on the LCD.

Examples:

- Current Mode
- Bluetooth Status
- Obstacle Distance
- Errors
- System Status

---

# Planned Features

Core:

- Motor PWM Control
- Bluetooth Communication
- LCD Interface
- EEPROM Logging
- Obstacle Detection
- Line Following

Extra:

- Eco / Normal / Sport Driving Modes
- Startup Self-Test
- Black Box Event Logger
- Diagnostics Screen

---

# Software Architecture

Project follows layered architecture.

Application

↓

HAL

↓

MCAL

↓

ATmega32

---

# Current Folder Structure

Includes/

    APP/
        VehicleControl/
        LineFollower/
        ObstacleAvoidance/
        Diagnostics/
        ModeManager/

    HAL/
        Bluetooth/
        EEPROM/
        IRSensor/
        Keypad/
        LCD/
        Motor/
        Ultrasonic/

    MCAL/
        ADC/
        DIO/
        I2C/
        INTR/
        TIMERS/
        UART/

    Bit_math.h
    StdTypes.h

PWM is implemented inside the TIMERS driver.

EEPROM refers to the external EEPROM located on the interface board.

---

# Suggested Responsibilities

VehicleControl

- Main application control
- Calls other application modules

LineFollower

- Reads IR sensors
- Calculates steering decisions

ObstacleAvoidance

- Reads HC-SR04
- Stops or reroutes vehicle

ModeManager

- Switch between:
    - Manual
    - Autonomous
    - Diagnostics

Diagnostics

- LCD updates
- Error reporting
- EEPROM logging

---

# Coding Philosophy

Application layer should NEVER manipulate hardware registers directly.

Example:

APP

↓

Motor_Forward(80);

↓

HAL Motor Driver

↓

MCAL PWM + DIO

↓

ATmega32 Registers

---

# GitHub Goals

Repository should look like a professional embedded project.

Include:

- README
- Architecture
- Hardware documentation
- State Machine
- Wiring diagrams
- Pictures
- Videos
- Datasheets

Commit frequently instead of one final upload.

---

# Future STM32 Migration

After the university project:

Reuse:

- Robot Chassis
- Motors
- Bluetooth
- Ultrasonic
- IR Sensors
- Battery
- Motor Driver

Replace:

ATmega32

↓

STM32 Blue Pill

Future portfolio projects:

- Embedded Dashboard
- Flight Controller Demo
- Automotive ECU Simulation
- CAN Bus Communication
- Sensor Fusion

---

# Current Status

Completed:

✔ Project idea finalized

✔ Software architecture designed

✔ Folder hierarchy designed

✔ GitHub repository planned

In Progress:

- Driver implementation
- Software architecture
- Hardware purchasing

Pending:

- Hardware integration
- System testing
- Final demonstration

---

# Design Principles

- Modular code
- Reusable drivers
- Layered architecture
- Clear separation between APP / HAL / MCAL
- Professional documentation
- Portfolio quality code

---

# Notes for Future AI Chats

Assume:

- Eclipse IDE
- ATmega32
- Embedded C
- No Arduino libraries
- Register-level programming
- Modular driver development
- GitHub documentation is important
- Code quality is prioritized over shortcuts

When suggesting code:

- Keep modules independent.
- Respect APP/HAL/MCAL architecture.
- Avoid writing everything inside main().
- Explain design decisions whenever possible.
I have one suggestion that will make this even more useful.
Create a Docs/ folder in your repository with:

Docs/
│
├── PROJECT_CONTEXT.md      ← This file
├── HARDWARE.md
├── SOFTWARE_ARCHITECTURE.md
├── STATE_MACHINE.md
├── WIRING.md
├── DEVELOPMENT_LOG.md
└── TODO.md
As the project evolves, you'll update these documents. If you ever start a new ChatGPT conversation or use another AI assistant, you can simply share the relevant Markdown files, and they'll have the context needed to help without you having to explain everything again. Given your goal of building a strong embedded portfolio, this kind of documentation is a habit that's well worth developing.


