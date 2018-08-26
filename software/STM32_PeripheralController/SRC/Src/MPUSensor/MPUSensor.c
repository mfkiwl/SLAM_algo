/*
 * MPUSensor.c
 *
 *  Created on: 23.08.2018
 *      Author: stefan
 */

#include "MPUSensor.h"

#define MPU_DEVICE_ADDRESS (0x68<<1)
#define MPU_REQUEST_DATA_COMMAND 0x3B
#define MPU_DLPF_REGISTER 0x1A
#define MPU_GYRO_CONFIG_REGISTER 0x1B
#define MPU_ACCEL_CONFIG_REGISTER 0x1C
#define MPU_PWR_MNGMT_REGISTER 0x6B
#define MPU_PAYLOAD_SIZE 14u

/* acceleration values: */
volatile LateralAccelValueRawType CurrentRaw_X_Accel;
volatile LateralAccelValueRawType CurrentRaw_Y_Accel;
volatile LateralAccelValueRawType CurrentRaw_Z_Accel;

volatile AngularAccelValueRawType CurrentRaw_XR_Accel;
volatile AngularAccelValueRawType CurrentRaw_YR_Accel;
volatile AngularAccelValueRawType CurrentRaw_ZR_Accel;

volatile LateralAccelValuePhysType CurrentPhys_X_Accel;
volatile LateralAccelValuePhysType CurrentPhys_Y_Accel;
volatile LateralAccelValuePhysType CurrentPhys_Z_Accel;

volatile AngularAccelValuePhysType CurrentPhys_XR_Accel;
volatile AngularAccelValuePhysType CurrentPhys_YR_Accel;
volatile AngularAccelValuePhysType CurrentPhys_ZR_Accel;

uint8_t payloadBufferI2C[15] = {0x0};

/* velocity values: */

void MPU_SetPhysLateralAccelerations(void)
{
    CurrentPhys_X_Accel = (((LateralAccelValuePhysType) CurrentRaw_X_Accel) * MPU_LATERAL_ACCEL_CONV_FACTOR);
    CurrentPhys_Y_Accel = (((LateralAccelValuePhysType) CurrentRaw_Y_Accel) * MPU_LATERAL_ACCEL_CONV_FACTOR);
    CurrentPhys_Z_Accel = (((LateralAccelValuePhysType) CurrentRaw_Z_Accel) * MPU_LATERAL_ACCEL_CONV_FACTOR);
}

void MPU_SetPhysAngularAccelerations(void)
{
    CurrentPhys_XR_Accel = (((AngularAccelValuePhysType) CurrentRaw_XR_Accel) * MPU_ANGULAR_ACCEL_CONV_FACTOR);
    CurrentPhys_YR_Accel = (((AngularAccelValuePhysType) CurrentRaw_YR_Accel) * MPU_ANGULAR_ACCEL_CONV_FACTOR);
    CurrentPhys_ZR_Accel = (((AngularAccelValuePhysType) CurrentRaw_ZR_Accel) * MPU_ANGULAR_ACCEL_CONV_FACTOR);
}

void MPU_GetPhysLateralAccelerations(LateralAccelValuePhysType * currentXAccel, LateralAccelValuePhysType * currentYAccel, LateralAccelValuePhysType * currentZAccel)
{
    (*currentXAccel) = CurrentPhys_X_Accel;
    (*currentYAccel) = CurrentPhys_Y_Accel;
    (*currentZAccel) = CurrentPhys_Z_Accel;
}

void MPU_GetPhysAngularAccelerations(LateralAccelValuePhysType * currentXRAccel, LateralAccelValuePhysType * currentYRAccel, LateralAccelValuePhysType * currentZRAccel)
{
    (*currentXRAccel) = CurrentPhys_XR_Accel;
    (*currentYRAccel) = CurrentPhys_YR_Accel;
    (*currentZRAccel) = CurrentPhys_ZR_Accel;
}


void MPU_Init(void)
{
    uint8_t payload[2] = {0x0, 0x0}; // disable power down mode
    HAL_I2C_Mem_Write(&hi2c1, MPU_DEVICE_ADDRESS, MPU_PWR_MNGMT_REGISTER, I2C_MEMADD_SIZE_8BIT, payload, 1, 50u);
    payload[0] = 0x1; // enable digital lowpass filter (stage 1)
    HAL_I2C_Mem_Write(&hi2c1, MPU_DEVICE_ADDRESS, MPU_DLPF_REGISTER, I2C_MEMADD_SIZE_8BIT, payload, 1, 50u);
    payload[0] = 0x8; // set sensivity to level 1 +-(4g)
    HAL_I2C_Mem_Write(&hi2c1, MPU_DEVICE_ADDRESS, MPU_GYRO_CONFIG_REGISTER, I2C_MEMADD_SIZE_8BIT, payload, 1, 50u);
    payload[0] = 0x8; // set sensivity to level 1 +-(4g)
    HAL_I2C_Mem_Write(&hi2c1, MPU_DEVICE_ADDRESS, MPU_ACCEL_CONFIG_REGISTER, I2C_MEMADD_SIZE_8BIT, payload, 1, 50u);
}

void MPU_ReadValues(void)
{
    uint8_t payload[1] = {MPU_REQUEST_DATA_COMMAND};
    uint16_t temp;
    int16_t *ptrToTemp = (int16_t *) &temp;
    /* Trigge Measurement from MPU */
    HAL_I2C_Mem_Read(&hi2c1, MPU_DEVICE_ADDRESS, MPU_REQUEST_DATA_COMMAND, I2C_MEMADD_SIZE_8BIT, payloadBufferI2C, 14, 50u);

    /* write to local data structures */
    temp = ((uint16_t)payloadBufferI2C[1] | (uint16_t)(payloadBufferI2C[0]) << 8);
    CurrentRaw_X_Accel = (*ptrToTemp);
    temp = ((uint16_t)payloadBufferI2C[3] | (uint16_t)(payloadBufferI2C[2]) << 8);
    CurrentRaw_Y_Accel = (*ptrToTemp);
    temp = ((uint16_t)payloadBufferI2C[5] | (uint16_t)(payloadBufferI2C[4]) << 8);
    CurrentRaw_Z_Accel = (*ptrToTemp);
    temp = ((uint16_t)payloadBufferI2C[9] | (uint16_t)(payloadBufferI2C[8]) << 8);
    CurrentRaw_XR_Accel = (*ptrToTemp);
    temp = ((uint16_t)payloadBufferI2C[11] | (uint16_t)(payloadBufferI2C[10]) << 8);
    CurrentRaw_YR_Accel = (*ptrToTemp);
    temp = ((uint16_t)payloadBufferI2C[13] | (uint16_t)(payloadBufferI2C[12]) << 8);
    CurrentRaw_ZR_Accel = (*ptrToTemp);

    MPU_SetPhysLateralAccelerations();
    MPU_SetPhysAngularAccelerations();

    if(CurrentPhys_X_Accel > 5.0f)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    }
    
}

