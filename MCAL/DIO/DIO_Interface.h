/*
 * Interface.h
 *
 *  Created on: 19 Jul 2026
 *      Author: mohib
 */
#include "../../StdTypes.h"
#ifndef MCAL_DIO_INTERFACE_H_
#define MCAL_DIO_INTERFACE_H_

#define DIO_PIN0 0
#define DIO_PIN1 1
#define DIO_PIN2 2
#define DIO_PIN3 3
#define DIO_PIN4 4
#define DIO_PIN5 5
#define DIO_PIN6 6
#define DIO_PIN7 7

#define DIO_PIN_INPUT 0
#define DIO_PIN_OUTPUT 1

#define DIO_PIN_HIGH 1
#define DIO_PIN_LOW 0

#define DIO_PORTA 0
#define DIO_PORTB 1
#define DIO_PORTC 2
#define DIO_PORTD 3

#define DIO_PORT_INPUT 0x00
#define DIO_PORT_OUTPUT 0xff

#define DIO_PORT_LOW 0x00
#define DIO_PORT_HIGH 0xff

void DIO_setPinDirection(uint8_t PortId, uint8_t pinId, uint8_t Pindirection);
void DIO_setPinValue(uint8_t PortId, uint8_t pinId, uint8_t PinValue);
void DIO_getPinValue(uint8_t PortId, uint8_t pinId, uint8_t* PinValue);
void DIO_togglePinValues(uint8_t PortId, uint8_t pinId);
void DIO_activePullUpRessistencePin(uint8_t PortId, uint8_t pinId);

void DIO_setPortDirection(uint8_t PortId,  uint8_t PortDirection);
void DIO_setPortValue(uint8_t PortId, uint8_t PortValue);
void DIO_getPortValue(uint8_t PortId, uint8_t* PortValue);
void DIO_togglePortValues(uint8_t PortId);
void DIO_activePullUpRessistencePort(uint8_t PortId);

#endif /* MCAL_DIO_INTERFACE_H_ */
