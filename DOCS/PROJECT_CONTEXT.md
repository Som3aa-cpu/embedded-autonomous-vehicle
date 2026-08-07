# === CHATGPT UPDATE (Current Session) ===

## Project Scope Update

The instructor confirmed that the robot must now include the following additional peripherals mounted on the robot itself:

- 16x2 LCD
- External EEPROM
- Status LEDs
- Mode Selection Push Button

These are now part of the final project requirements rather than optional additions.

---

## Updated Robot Features

### Manual Mode
- Bluetooth control via HC-05
- Green LED ON
- LCD displays:
  Mode: Manual

### Autonomous Mode
- Line Following
- Obstacle Detection
- Obstacle Avoidance
- Blue LED ON
- LCD displays:
  Mode: Auto

### Obstacle Status
When an obstacle is detected:
- Red LED ON
- LCD displays warning
- Robot stops or performs avoidance maneuver

---

## LCD Responsibilities

The LCD acts as the Human Machine Interface (HMI).

It should display:

Startup Screen

Initializing...

Current Mode

Manual / Auto

Bluetooth Status

Connected / Disconnected

Obstacle Status

Obstacle Detected

Distance Measurements

Optional IMU Diagnostics

Pitch / Roll

---

## EEPROM Responsibilities

The EEPROM is no longer optional.

It should store persistent configuration values.

Suggested memory map:

0x00 -> Last Operating Mode

0x01 -> Default Motor Speed

0x02 -> Obstacle Detection Distance

0x03 -> IR Calibration Left

0x04 -> IR Calibration Center

0x05 -> IR Calibration Right

On startup the application loads these values.

Whenever settings change they should be written back to EEPROM.

---

## MPU6050 Responsibilities

The MPU6050 should provide practical functionality rather than only displaying raw values.

Planned features:

- Robot Tilt Detection
- Collision Detection
- Motion Monitoring
- Diagnostics Screen
- Future STM32 compatibility

Diagnostics screen example:

Pitch

Roll

Acceleration

---

## I2C Architecture

Both the MPU6050 and External EEPROM share the same I2C bus.

ATmega32 Hardware TWI Pins

PC0 -> SCL

PC1 -> SDA

The MCU selects the target device using its I2C address.

Example:

MPU6050

Address:
0x68

EEPROM (24Cxx)

Typical Address:
0x50

Only one device responds at a time.

This allows unlimited expansion without consuming additional GPIO pins.

---

## Updated HAL Modules

HAL/

Motor/

Bluetooth/

Ultrasonic/

IRSensor/

LCD/

EEPROM/

MPU6050/

LED/

Button/

---

## Updated APP Modules

APP/

VehicleControl/

ModeManager/

DisplayManager/

SettingsManager/

BluetoothControl/

ObstacleAvoidance/

LineFollower/

Diagnostics/

---

## DisplayManager

Responsible for ALL LCD updates.

Application modules should never access the LCD driver directly.

Example APIs:

Display_ShowMode()

Display_ShowDistance()

Display_ShowObstacle()

Display_ShowBluetooth()

Display_ShowDiagnostics()

---

## SettingsManager

Owns the EEPROM.

Example APIs:

Settings_Load()

Settings_Save()

Settings_Reset()

No other module should access EEPROM directly.

---

## LED Usage

Green LED

Manual Mode

Blue LED

Autonomous Mode

Red LED

Obstacle Detected

---

## Button Functionality

Single push button cycles between modes.

Manual

↓

Autonomous

↓

Manual

The selected mode should immediately be saved to EEPROM.

---

## Power Distribution

Battery (2x18650)

↓

Power Switch

↓

Split

↓

ATmega32 Development Board

↓

On-board 5V Regulator

↓

5V Rail

↓

HC05

LCD

EEPROM

MPU6050

IR Sensors

HC-SR04

Second Branch

↓

L298N Motor Driver

All modules share a common Battery Ground.

---

## Wiring Philosophy

Battery powers:

- ATmega32 Development Board
- L298N

ATmega Board supplies regulated 5V to:

- LCD
- EEPROM
- HC05
- HC-SR04
- MPU6050
- IR Sensors

Every module shares the same Ground reference.

---

## Current Hardware Bus Allocation

UART

HC-05

I2C

MPU6050

EEPROM

PWM

Motor Driver (L298N)

GPIO

IR Sensors

Ultrasonic Trigger/Echo

LEDs

Mode Button

LCD Control

---

## Development Status

Completed

✓ MCAL Drivers

✓ HAL Drivers

✓ Individual Driver Testing

✓ Hardware Architecture

✓ Software Architecture

✓ Power Distribution Design

✓ Wiring Strategy

Next Stage

System Integration

↓

Application Layer

↓

Mode Manager

↓

Display Manager

↓

Settings Manager

↓

Complete Robot Testing

---

## Development Strategy

Drivers have already been verified individually.

The remaining work focuses on integrating the complete system using the layered architecture rather than writing additional low-level drivers.

The project should continue following strict separation:

APP

↓

HAL

↓

MCAL

↓

Registers

No application code should directly access MCU registers.