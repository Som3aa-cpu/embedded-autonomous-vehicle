/*
 * I2C_private.h
 *
 *  Created on: 3 Aug 2026
 *      Author: mohib
 */

#ifndef MCAL_I2C_I2C_PRIVATE_H_
#define MCAL_I2C_I2C_PRIVATE_H_

#define TWBR_REG    (*((volatile uint8_t*) 0x20))
#define TWSR_REG    (*((volatile uint8_t*) 0x21))
#define TWAR_REG    (*((volatile uint8_t*) 0x22))
#define TWDR_REG    (*((volatile uint8_t*) 0x23))
#define TWCR_REG    (*((volatile uint8_t*) 0x56))

// TWCR Bits
#define TWIE_BIT    0
#define TWEN_BIT    2
#define TWWC_BIT    3
#define TWSTO_BIT   4
#define TWSTA_BIT   5
#define TWEA_BIT    6
#define TWINT_BIT   7

// TWSR Bits
#define TWPS0_BIT   0
#define TWPS1_BIT   1

// TWI Interrupt Vector (Vector 25 in ATmega32 datasheet)
#define I2C_vect    __vector_19
void I2C_vect(void) __attribute__((signal,used));

#endif /* MCAL_I2C_I2C_PRIVATE_H_ */
