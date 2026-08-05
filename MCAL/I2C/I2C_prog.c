/*
 * I2C_prog.c
 *
 *  Created on: 3 Aug 2026
 *      Author: mohib
 */
#include "../../StdTypes.h"
#include "../../Bit_math.h"
#include "I2C_private.h"
#include "I2C_interface.h"
#include "I2C_config.h"
static PF I2C_CallBackFunc = NULL;

void I2C_vidInit(void)
{
	TWBR_REG = ((F_CPU / I2C_SCL_CLOCK_FREQ) - 16) / (2 * I2C_PRESCALER);
#if I2C_PRESCALER == 1
	Clr_Bit(TWSR_REG, TWPS0_BIT);
	Clr_Bit(TWSR_REG, TWPS1_BIT);
#elif I2C_PRESCALER == 4
	Set_Bit(TWSR_REG, TWPS0_BIT);
	Clr_Bit(TWSR_REG, TWPS1_BIT);
#elif I2C_PRESCALER == 16
	Clr_Bit(TWSR_REG, TWPS0_BIT);
	Set_Bit(TWSR_REG, TWPS1_BIT);
#elif I2C_PRESCALER == 64
	Set_Bit(TWSR_REG, TWPS0_BIT);
	Set_Bit(TWSR_REG, TWPS1_BIT);
#endif
	TWAR_REG = (I2C_SLAVE_ADDRESS << 1);
}

void I2C_vidEnable(void) {
	Set_Bit(TWCR_REG, TWEN_BIT);
}

void I2C_vidDisable(void) {
	Clr_Bit(TWCR_REG, TWEN_BIT);
}

void I2C_vidEnableIreq(void) {
	Set_Bit(TWCR_REG, TWIE_BIT);
}

void I2C_vidDisableIreq(void) {
	Clr_Bit(TWCR_REG, TWIE_BIT);
}

void I2C_vidSetCallBack(PF pfCallbackCpy) {
	if (pfCallbackCpy != NULL) {
		I2C_CallBackFunc = pfCallbackCpy;
	}
}

void I2C_vidStartCond(void) {
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWSTA_BIT) | (1 << TWEN_BIT);
	I2C_vidWait();
}

void I2C_vidRepStartCond(void) {
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWSTA_BIT) | (1 << TWEN_BIT);
	I2C_vidWait();
}

void I2C_vidStopCond(void) {
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWSTO_BIT) | (1 << TWEN_BIT);
}

void I2C_vidSendData(uint8_t u8DataCpy) {
	TWDR_REG = u8DataCpy;
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWEN_BIT);
	I2C_vidWait();
}

uint8_t I2C_u8GetDataAck(void) {
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWEA_BIT) | (1 << TWEN_BIT);
	I2C_vidWait();
	return TWDR_REG;
}

uint8_t I2C_u8GetDataNoAck(void) {
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWEN_BIT);
	I2C_vidWait();
	return TWDR_REG;
}

void I2C_vidStartReceive_ACK(void) {
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWEA_BIT) | (1 << TWEN_BIT) | (1 << TWIE_BIT);
}

void I2C_vidStartReceive_N_ACK(void) {
	TWCR_REG = (1 << TWINT_BIT) | (1 << TWEN_BIT) | (1 << TWIE_BIT);
}

uint8_t I2C_u8ReadData(void) {
	return TWDR_REG;
}

uint8_t I2C_u8GetStatusCode(void) {
	return (TWSR_REG & 0xF8);
}

void I2C_vidWait(void) {
	while (Get_Bit(TWCR_REG, TWINT_BIT) == 0);
}


void I2C_vect(void) {
	if (I2C_CallBackFunc != NULL) {
		I2C_CallBackFunc();
	}
}
