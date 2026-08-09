/*
 * MPU6050_priv.h
 *
 *  Created on: 6 Aug 2026
 *      Author: mohib
 */

#ifndef HAL_MPU6050_MPU6050_PRIV_H_
#define HAL_MPU6050_MPU6050_PRIV_H_

/* Register map */
#define MPU6050_REG_SMPLRT_DIV     0x19U
#define MPU6050_REG_CONFIG         0x1AU
#define MPU6050_REG_GYRO_CONFIG    0x1BU
#define MPU6050_REG_ACCEL_CONFIG   0x1CU
#define MPU6050_REG_ACCEL_XOUT_H   0x3BU
#define MPU6050_REG_PWR_MGMT_1     0x6BU
#define MPU6050_REG_WHO_AM_I       0x75U

#define MPU6050_WHO_AM_I_VALUE     0x68

/* PWR_MGMT_1: clear sleep bit, use internal 8MHz oscillator */
#define MPU6050_PWR_MGMT_1_WAKE    0x00U

/* Full scale range selects (0 = most sensitive range for each) */
#define MPU6050_ACCEL_FS_2G        0x00U   /* +-2g,  16384 LSB/g   */
#define MPU6050_GYRO_FS_250DPS     0x00U   /* +-250dps, 131 LSB/(deg/s) */

#define MPU6050_ACCEL_SENSITIVITY  16384.0f
#define MPU6050_GYRO_SENSITIVITY   131.0f

#define MPU6050_RAW_BYTES          14U     /* Accel(6) + Temp(2) + Gyro(6) */

#define MPU6050_OK                 0U
#define MPU6050_NOK                1U



#endif /* HAL_MPU6050_MPU6050_PRIV_H_ */
