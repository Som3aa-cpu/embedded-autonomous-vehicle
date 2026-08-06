# ATmega32 Pin Mapping — Autonomous Vehicle Platform

## PORTA — IR Sensor Array
| Pin | Function |
|---|---|
| PA0–PA4 | 5-channel IR line sensor (digital in) |
| PA5–PA7 | Free |

## PORTB
| Pin | Function |
|---|---|
| PB3 (OC0) | Left motor PWM (ENA) |
| PB4 | Ultrasonic Trigger |
| PB0–PB2 | Free |
| PB5 (MOSI) | Reserved — ISP programmer |
| PB6 (MISO) | Reserved — ISP programmer |
| PB7 (SCK) | Reserved — ISP programmer |

## PORTC
| Pin | Function |
|---|---|
| PC0 (SCL) | Hardware I2C — MPU6050 |
| PC1 (SDA) | Hardware I2C — MPU6050 |
| PC4 | Left motor direction IN1 |
| PC5 | Left motor direction IN2 |
| PC6 | Right motor direction IN1 |
| PC7 | Right motor direction IN2 |
| PC2, PC3 | Free |

## PORTD
| Pin | Function |
|---|---|
| PD0 (RXD) | HC-05 Bluetooth TXD → ATmega RXD |
| PD1 (TXD) | ATmega TXD → HC-05 Bluetooth RXD |
| PD5 (OC1A) | Right motor PWM (ENB) |
| PD6 (ICP1) | Ultrasonic Echo (Timer1 Input Capture) |
| PD2 (INT0), PD3 (INT1) | Free — available for future external interrupts |
| PD4, PD7 | Free |

## Notes
- PB5–PB7 are shared with the ISP programmer header; avoid using them for anything else if you plan to reprogram in-circuit.
- PC0/PC1 are the ATmega32's fixed hardware TWI pins — do not reassign them; the MPU6050 (and any future EEPROM) must connect here.
- HC-05 logic is 3.3V; check your specific module's RXD tolerance for 5V input from the ATmega TXD line, and add a voltage divider/level shifter if it isn't tolerant.
- Ultrasonic Echo uses ICP1 (PD6) so `ICU_Init()` / `ICU_GetCapturedValue()` can time the pulse in hardware instead of software polling.
