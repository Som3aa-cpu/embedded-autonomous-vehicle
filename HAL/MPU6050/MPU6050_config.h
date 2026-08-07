/*
 * MPU6050_config.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_MPU6050_MPU6050_CONFIG_H_
#define HAL_MPU6050_MPU6050_CONFIG_H_


#define MPU6050_SCL_PORT       DIO_PORTC
#define MPU6050_SCL_PIN        DIO_PIN0

#define MPU6050_SDA_PORT       DIO_PORTC
#define MPU6050_SDA_PIN        DIO_PIN1

/* MPU6050 default I2C 7-bit slave address (AD0 pin tied low) */
#define MPU6050_I2C_ADDRESS    0x68U


#endif /* HAL_MPU6050_MPU6050_CONFIG_H_ */
