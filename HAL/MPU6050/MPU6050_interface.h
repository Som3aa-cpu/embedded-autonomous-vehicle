/*
 * MPU6050_interface.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_MPU6050_MPU6050_INTERFACE_H_
#define HAL_MPU6050_MPU6050_INTERFACE_H_

#include "../../StdTypes.h"

typedef struct
{
    sint16_t AccelX;
    sint16_t AccelY;
    sint16_t AccelZ;
    sint16_t Temp;
    sint16_t GyroX;
    sint16_t GyroY;
    sint16_t GyroZ;
} MPU6050_RawData_t;

/* Wakes the sensor and configures sample rate, accel/gyro full scale range */
void MPU6050_vidInit(void);

/* Reads WHO_AM_I and confirms it matches the expected MPU6050 value
 * Returns: MPU6050_OK / MPU6050_NOK */
uint8_t MPU6050_u8CheckConnection(void);

/* Burst-reads accel, temp, and gyro registers in one transaction
 * Returns: MPU6050_OK / MPU6050_NOK */
uint8_t MPU6050_u8ReadRawData(MPU6050_RawData_t* Copy_pstData);

/* Converts raw accel counts to g, based on the configured full scale range */
void MPU6050_vidGetAccel_g(const MPU6050_RawData_t* Copy_pstRaw,
                            float* Copy_pfAx, float* Copy_pfAy, float* Copy_pfAz);

/* Converts raw gyro counts to degrees/second, based on the configured full scale range */
void MPU6050_vidGetGyro_dps(const MPU6050_RawData_t* Copy_pstRaw,
                             float* Copy_pfGx, float* Copy_pfGy, float* Copy_pfGz);

/* Converts raw temp reading to degrees Celsius (MPU6050 datasheet formula) */
float MPU6050_f32GetTemp_C(const MPU6050_RawData_t* Copy_pstRaw);



#endif /* HAL_MPU6050_MPU6050_INTERFACE_H_ */
