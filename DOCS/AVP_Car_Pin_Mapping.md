# AVP-Car Complete Pin Mapping & Wiring Guide

**Project:** Embedded Autonomous Vehicle Platform  
**MCU:** ATmega32 @ 11.0592 MHz  
**Wireless Co-Processor:** ESP32 (BLE Nordic UART Service)  
**Drive:** Differential 4WD via L298N  
**Date:** 2026-08-09

---

## 1. ATmega32 Pin Assignment

### PORTA — Analog Sensors + Status LEDs

| Pin | Function | Direction | Connected To | Notes |
|-----|----------|-----------|--------------|-------|
| **PA0** | IR Sensor Ch 0 | Input | 5-Channel IR Array | Far Right line sensor |
| **PA1** | IR Sensor Ch 1 | Input | 5-Channel IR Array | Right line sensor |
| **PA2** | IR Sensor Ch 2 | Input | 5-Channel IR Array | Center line sensor |
| **PA3** | IR Sensor Ch 3 | Input | 5-Channel IR Array | Left line sensor |
| **PA4** | IR Sensor Ch 4 | Input | 5-Channel IR Array | Far Left line sensor |
| **PA5** | **LED_RED** | Output | Red LED (Manual Mode) | 330Ω resistor to GND |
| **PA6** | **LED_GREEN** | Output | Green LED (Auto Mode) | 330Ω resistor to GND |
| **PA7** | **LED_BLUE** | Output | Blue LED (Obstacle Detected) | 330Ω resistor to GND |

### PORTB — Ultrasonic + ISP

| Pin | Function | Direction | Connected To | Notes |
|-----|----------|-----------|--------------|-------|
| **PB0** | FREE | — | — | Available for future use |
| **PB1** | FREE | — | — | Available for future use |
| **PB2** | FREE | — | — | Available for future use |
| **PB3** | FREE | — | — | Timer0/OC0 free; available for servo/buzzer |
| **PB4** | **ULTRASONIC_TRIG** | Output | HC-SR04 Trigger | 10µs pulse to start measurement |
| **PB5** | MOSI | Output | ISP Programmer | Reserved — do not reassign |
| **PB6** | MISO | Input | ISP Programmer | Reserved — do not reassign |
| **PB7** | SCK | Output | ISP Programmer | Reserved — do not reassign |

### PORTC — I2C Bus + Motor Direction

| Pin | Function | Direction | Connected To | Notes |
|-----|----------|-----------|--------------|-------|
| **PC0** | **SCL** | I/O | MPU6050, SSD1306, AT24C08 | Hardware TWI — fixed pin |
| **PC1** | **SDA** | I/O | MPU6050, SSD1306, AT24C08 | Hardware TWI — fixed pin |
| **PC2** | FREE | — | — | Available for future use |
| **PC3** | FREE | — | — | Available for future use |
| **PC4** | **MOTOR_L_IN1** | Output | L298N IN1 | Left motor direction |
| **PC5** | **MOTOR_L_IN2** | Output | L298N IN2 | Left motor direction |
| **PC6** | **MOTOR_R_IN1** | Output | L298N IN3 | Right motor direction |
| **PC7** | **MOTOR_R_IN2** | Output | L298N IN4 | Right motor direction |

### PORTD — UART + PWM + Ultrasonic Echo

| Pin | Function | Direction | Connected To | Notes |
|-----|----------|-----------|--------------|-------|
| **PD0** | **RXD** | Input | ESP32 GPIO17 (TX) | 3.3V → 5V, safe |
| **PD1** | **TXD** | Output | ESP32 GPIO16 (RX) | **5V → 3.3V LEVEL SHIFT REQUIRED** |
| **PD2** | FREE / INT0 | — | — | External interrupt available |
| **PD3** | FREE / INT1 | — | — | External interrupt available |
| **PD4** | **OC1B** | Output | L298N ENA (Left PWM) | Timer1 Fast PWM, channel B |
| **PD5** | **OC1A** | Output | L298N ENB (Right PWM) | Timer1 Fast PWM, channel A |
| **PD6** | **ICP1** | Input | HC-SR04 Echo | Timer1 Input Capture — hardware pulse width |
| **PD7** | FREE | — | — | Available for future use |

---

## 2. ESP32 Pin Assignment

| ESP32 Pin | Function | Direction | Connected To | Notes |
|-----------|----------|-----------|--------------|-------|
| **GPIO16 (RX2)** | UART RX | Input | ATmega32 PD1 (TXD) | **3.3V max — use level shifter** |
| **GPIO17 (TX2)** | UART TX | Output | ATmega32 PD0 (RXD) | 3.3V → 5V, safe for ATmega32 |
| **GND** | Common Ground | — | ATmega32 GND | Must be shared |
| **5V (VIN)** | Power Input | Input | ATmega32 5V Rail | Powered from regulated 5V |

> **ESP32 BLE Service:** Nordic UART Service (NUS)  
> **Device Name:** `AVP-Car`  
> **UART Baud:** 115200, 8-N-1

---

## 3. Motor Driver (L298N) Wiring

| L298N Pin | Connected To | Function |
|-----------|--------------|----------|
| **ENA** | ATmega32 PD4 (OC1B) | Left motor group PWM |
| **IN1** | ATmega32 PC4 | Left motor direction IN1 |
| **IN2** | ATmega32 PC5 | Left motor direction IN2 |
| **IN3** | ATmega32 PC6 | Right motor direction IN1 |
| **IN4** | ATmega32 PC7 | Right motor direction IN2 |
| **ENB** | ATmega32 PD5 (OC1A) | Right motor group PWM |
| **OUT1 / OUT2** | Left Front + Left Rear motors | Parallel, same polarity |
| **OUT3 / OUT4** | Right Front + Right Rear motors | Parallel, same polarity |
| **VIN** | Battery positive (7.4V) | Motor power |
| **GND** | Battery GND + ATmega32 GND | Common ground |
| **+5V (optional)** | Unconnected | On-board 5V regulator output — not needed |

> **Motor wiring note:** One left motor had reversed leads in hardware. Re-wire physically rather than changing driver polarity.

---

## 4. Sensor Wiring

### HC-SR04 Ultrasonic

| HC-SR04 Pin | Connected To | Notes |
|-------------|--------------|-------|
| **VCC** | 5V Rail | From ATmega32 board regulator |
| **GND** | Common GND | |
| **Trig** | ATmega32 PB4 | 10µs trigger pulse |
| **Echo** | ATmega32 PD6 (ICP1) | Timer1 Input Capture — hardware timed |

### 5-Channel IR Line Sensor

| IR Module Pin | Connected To |
|---------------|--------------|
| **VCC** | 5V Rail |
| **GND** | Common GND |
| **Ch0 (Far Right)** | ATmega32 PA0 |
| **Ch1 (Right)** | ATmega32 PA1 |
| **Ch2 (Center)** | ATmega32 PA2 |
| **Ch3 (Left)** | ATmega32 PA3 |
| **Ch4 (Far Left)** | ATmega32 PA4 |

### I2C Bus Devices (Shared)

| Device | Address | Connected To |
|--------|---------|--------------|
| **MPU6050 IMU** | 0x68 | PC0 (SCL), PC1 (SDA) |
| **SSD1306 OLED** | 0x3C | PC0 (SCL), PC1 (SDA) |
| **AT24C08 EEPROM** | 0x50 | PC0 (SCL), PC1 (SDA) |

> **Pull-up resistors:** 4.7kΩ on both SCL and SDA lines to 5V. Required for I2C bus integrity.

---

## 5. Status LEDs Wiring

| LED Color | ATmega32 Pin | Resistor | Behavior |
|-----------|--------------|----------|----------|
| **Red** | PA5 | 330Ω → GND | ON in **Manual** mode |
| **Green** | PA6 | 330Ω → GND | ON in **Autonomous** mode |
| **Blue** | PA7 | 330Ω → GND | ON when **obstacle detected** |

### LED Logic Summary

| System State | Red (Manual) | Green (Auto) | Blue (Obstacle) |
|--------------|--------------|--------------|-----------------|
| **Manual Mode** | **ON** | OFF | ON if obstacle < threshold |
| **Autonomous Mode** | OFF | **ON** | ON if obstacle < threshold |
| **Diagnostics Mode** | **ON** | **ON** | **ON** |
| **Stop / Idle** | OFF | OFF | OFF |

> **Active-high logic:** `DIO_vidSetPinValue(PORTA, PINx, HIGH)` turns LED on.

---

## 6. Power Distribution

```
2×18650 Battery (7.4V nominal)
        │
        ├── Power Switch
        │       │
        │       ├──→ ATmega32 Board VIN
        │       │       └── On-board 5V Regulator
        │       │               ├──→ 5V Rail
        │       │               │       ├──→ HC-05 (removed)
        │       │               │       ├──→ HC-SR04 VCC
        │       │               │       ├──→ IR Sensor VCC
        │       │               │       ├──→ MPU6050 VCC
        │       │               │       ├──→ SSD1306 OLED VCC
        │       │               │       ├──→ AT24C08 EEPROM VCC
        │       │               │       └──→ ESP32 VIN (5V)
        │       │               │
        │       └──→ L298N VIN (motor power)
        │
        └──→ Common GND (ALL devices share this ground)
```

> **Critical:** Battery GND, L298N GND, ATmega32 GND, ESP32 GND, and all sensor GNDs must be tied together.

---

## 7. Level Shifter Detail (ATmega32 TX → ESP32 RX)

ATmega32 PD1 outputs **5V logic**. ESP32 GPIO16 accepts **3.3V max**.

**Voltage Divider (simple, sufficient for 115200 baud):**

```
ATmega32 PD1 (5V) ───[1kΩ]───┬───[2kΩ]─── GND
                               │
                               └──→ ESP32 GPIO16 (RX)  ≈ 3.3V
```

**Alternative: BSS138 MOSFET Level Shifter Module**

| Level Shifter Pin | Connected To |
|-------------------|--------------|
| **HV** | 5V Rail |
| **HV1** | ATmega32 PD1 |
| **GND** | Common GND |
| **LV** | 3.3V (from ESP32 3.3V pin or external regulator) |
| **LV1** | ESP32 GPIO16 |

> The ESP32 TX → ATmega32 RX line does **not** need a level shifter (3.3V is read as logic HIGH by the 5V ATmega32).

---

## 8. Timer Resource Map

| Timer | Usage | Pins | Notes |
|-------|-------|------|-------|
| **Timer0** | Free | PB3 (OC0) | Available for servo, buzzer, or soft PWM |
| **Timer1** | Motor PWM + Ultrasonic ICU | PD4 (OC1B), PD5 (OC1A), PD6 (ICP1) | Fast PWM Mode 14, TOP=ICR1=19999, prescaler 8, ~69 Hz |
| **Timer2** | Free | — | Available for system tick or delay |

---

## 9. Complete Pin Usage Summary

| Port | Pins Used | Pins Free |
|------|-----------|-----------|
| **PORTA** | PA0–PA4 (IR), PA5–PA7 (LEDs) | — |
| **PORTB** | PB4 (Trig), PB5–PB7 (ISP) | PB0–PB3 |
| **PORTC** | PC0–PC1 (I2C), PC4–PC7 (Motor Dir) | PC2–PC3 |
| **PORTD** | PD0–PD1 (UART), PD4–PD5 (PWM), PD6 (Echo) | PD2–PD3, PD7 |

**Free pins available:** PB0–PB3, PC2–PC3, PD2–PD3, PD7 (11 pins total)

---

## 10. Software Integration for LEDs

### DIO HAL Calls (add to your existing DIO driver or use inline)

```c
/* In ModeManager_program.c — add LED updates */

#include "DIO_interface.h"

#define LED_RED_PIN     PIN5
#define LED_GREEN_PIN   PIN6
#define LED_BLUE_PIN    PIN7
#define LED_PORT        PORTA

void ModeManager_vidSetMode(u8 copy_u8Mode) {
    if (copy_u8Mode <= MODE_DIAGNOSTICS) {
        ObstacleAvoidance_vidReset();
        ModeManager_u8CurrentMode = copy_u8Mode;

        /* Update mode LEDs */
        DIO_vidSetPinValue(LED_PORT, LED_RED_PIN,   LOW);
        DIO_vidSetPinValue(LED_PORT, LED_GREEN_PIN, LOW);

        switch (copy_u8Mode) {
            case MODE_MANUAL:
                DIO_vidSetPinValue(LED_PORT, LED_RED_PIN, HIGH);
                break;
            case MODE_AUTONOMOUS:
                DIO_vidSetPinValue(LED_PORT, LED_GREEN_PIN, HIGH);
                break;
            case MODE_DIAGNOSTICS:
                DIO_vidSetPinValue(LED_PORT, LED_RED_PIN,   HIGH);
                DIO_vidSetPinValue(LED_PORT, LED_GREEN_PIN, HIGH);
                DIO_vidSetPinValue(LED_PORT, LED_BLUE_PIN,  HIGH);
                break;
        }
    }
}
```

### Obstacle LED Update (add to ObstacleAvoidance or VehicleControl)

```c
/* In VehicleControl_vidRun() or ObstacleAvoidance state machine */

u8 u8Obstacle = ObstacleAvoidance_u8IsObstacleDetected();

if (ModeManager_u8GetMode() != MODE_DIAGNOSTICS) {
    /* Blue LED = obstacle warning (not in diag — diag forces all on) */
    DIO_vidSetPinValue(LED_PORT, LED_BLUE_PIN, u8Obstacle ? HIGH : LOW);
}
```

> **Initialization:** Set PA5–PA7 as outputs in your `DIO_vidInit()` or `main()` bootstrap before `VehicleControl_vidInit()`.

---

*Document generated for ATmega32 @ 11.0592 MHz, F_CPU = 11059200UL, I2C pull-ups 4.7kΩ.*
