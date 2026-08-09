#include "MPU6050_interface.h"
#include "MPU6050_config.h"
#include "MPU6050_priv.h"
#include "../../MCAL/I2C/I2C_interface.h"

/* Helper: safe stop + return NOK */
#define MPU6050_BUS_ERROR()     do { I2C_vidStopCond(); return MPU6050_NOK; } while(0)

/* Writes a single register */
static uint8_t MPU6050_u8WriteReg(uint8_t Copy_u8RegAddr, uint8_t Copy_u8Data)
{
    I2C_vidStartCond();
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_START) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData((uint8_t)(MPU6050_I2C_ADDRESS << 1) | 0U);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData(Copy_u8RegAddr);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_DATA_ACK) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData(Copy_u8Data);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_DATA_ACK) { MPU6050_BUS_ERROR(); }

    I2C_vidStopCond();
    return MPU6050_OK;
}

/* Reads a single register */
static uint8_t MPU6050_u8ReadReg(uint8_t Copy_u8RegAddr, uint8_t* Copy_pu8Data)
{
    I2C_vidStartCond();
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_START) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData((uint8_t)(MPU6050_I2C_ADDRESS << 1) | 0U);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData(Copy_u8RegAddr);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_DATA_ACK) { MPU6050_BUS_ERROR(); }

    I2C_vidRepStartCond();
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_REP_START) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData((uint8_t)(MPU6050_I2C_ADDRESS << 1) | 1U);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MR_SLAR_ACK) { MPU6050_BUS_ERROR(); }

    *Copy_pu8Data = I2C_u8GetDataNoAck();
    I2C_vidStopCond();

    return MPU6050_OK;
}

/* Burst-reads Copy_u8Len bytes */
static uint8_t MPU6050_u8ReadBurst(uint8_t Copy_u8StartReg, uint8_t* Copy_pu8Buffer, uint8_t Copy_u8Len)
{
    uint8_t Local_u8Index;

    I2C_vidStartCond();
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_START) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData((uint8_t)(MPU6050_I2C_ADDRESS << 1) | 0U);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_SLAW_ACK) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData(Copy_u8StartReg);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_DATA_ACK) { MPU6050_BUS_ERROR(); }

    I2C_vidRepStartCond();
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MT_REP_START) { MPU6050_BUS_ERROR(); }

    I2C_vidSendData((uint8_t)(MPU6050_I2C_ADDRESS << 1) | 1U);
    I2C_vidWait();
    if (I2C_u8GetStatusCode() != MR_SLAR_ACK) { MPU6050_BUS_ERROR(); }

    for (Local_u8Index = 0U; Local_u8Index < Copy_u8Len; Local_u8Index++)
    {
        if (Local_u8Index < (Copy_u8Len - 1U))
        {
            Copy_pu8Buffer[Local_u8Index] = I2C_u8GetDataAck();
        }
        else
        {
            Copy_pu8Buffer[Local_u8Index] = I2C_u8GetDataNoAck();
        }
    }

    I2C_vidStopCond();
    return MPU6050_OK;
}

void MPU6050_vidInit(void)
{
    I2C_vidInit();
    I2C_vidEnable();

    MPU6050_u8WriteReg(MPU6050_REG_PWR_MGMT_1, MPU6050_PWR_MGMT_1_WAKE);
    MPU6050_u8WriteReg(MPU6050_REG_SMPLRT_DIV, 0x07U);
    MPU6050_u8WriteReg(MPU6050_REG_CONFIG, 0x00U);
    MPU6050_u8WriteReg(MPU6050_REG_GYRO_CONFIG, MPU6050_GYRO_FS_250DPS);
    MPU6050_u8WriteReg(MPU6050_REG_ACCEL_CONFIG, MPU6050_ACCEL_FS_2G);
}

uint8_t MPU6050_u8CheckConnection(void)
{
    uint8_t Local_u8WhoAmI = 0U;

    if (MPU6050_u8ReadReg(MPU6050_REG_WHO_AM_I, &Local_u8WhoAmI) != MPU6050_OK)
    {
        return MPU6050_NOK;
    }

    /* 0x68 = genuine MPU6050, 0x73 = MPU6500/9250 clone */
    return (Local_u8WhoAmI == MPU6050_WHO_AM_I_VALUE) ? MPU6050_OK : MPU6050_NOK;
}

uint8_t MPU6050_u8ReadRawData(MPU6050_RawData_t* Copy_pstData)
{
    uint8_t Local_au8Buffer[MPU6050_RAW_BYTES];

    if (MPU6050_u8ReadBurst(MPU6050_REG_ACCEL_XOUT_H, Local_au8Buffer, MPU6050_RAW_BYTES) != MPU6050_OK)
    {
        return MPU6050_NOK;
    }

    Copy_pstData->AccelX = (sint16_t)((Local_au8Buffer[0]  << 8) | Local_au8Buffer[1]);
    Copy_pstData->AccelY = (sint16_t)((Local_au8Buffer[2]  << 8) | Local_au8Buffer[3]);
    Copy_pstData->AccelZ = (sint16_t)((Local_au8Buffer[4]  << 8) | Local_au8Buffer[5]);
    Copy_pstData->Temp   = (sint16_t)((Local_au8Buffer[6]  << 8) | Local_au8Buffer[7]);
    Copy_pstData->GyroX  = (sint16_t)((Local_au8Buffer[8]  << 8) | Local_au8Buffer[9]);
    Copy_pstData->GyroY  = (sint16_t)((Local_au8Buffer[10] << 8) | Local_au8Buffer[11]);
    Copy_pstData->GyroZ  = (sint16_t)((Local_au8Buffer[12] << 8) | Local_au8Buffer[13]);

    return MPU6050_OK;
}

void MPU6050_vidGetAccel_g(const MPU6050_RawData_t* Copy_pstRaw,
                            float* Copy_pfAx, float* Copy_pfAy, float* Copy_pfAz)
{
    *Copy_pfAx = (float)Copy_pstRaw->AccelX / MPU6050_ACCEL_SENSITIVITY;
    *Copy_pfAy = (float)Copy_pstRaw->AccelY / MPU6050_ACCEL_SENSITIVITY;
    *Copy_pfAz = (float)Copy_pstRaw->AccelZ / MPU6050_ACCEL_SENSITIVITY;
}

void MPU6050_vidGetGyro_dps(const MPU6050_RawData_t* Copy_pstRaw,
                             float* Copy_pfGx, float* Copy_pfGy, float* Copy_pfGz)
{
    *Copy_pfGx = (float)Copy_pstRaw->GyroX / MPU6050_GYRO_SENSITIVITY;
    *Copy_pfGy = (float)Copy_pstRaw->GyroY / MPU6050_GYRO_SENSITIVITY;
    *Copy_pfGz = (float)Copy_pstRaw->GyroZ / MPU6050_GYRO_SENSITIVITY;
}

float MPU6050_f32GetTemp_C(const MPU6050_RawData_t* Copy_pstRaw)
{
    return ((float)Copy_pstRaw->Temp / 340.0f) + 36.53f;
}
