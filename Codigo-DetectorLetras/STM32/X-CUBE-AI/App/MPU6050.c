/*
 * MPU6500_6050.c
 *
 *  Created on: Aug 29, 2025
 *      Author: Melissa
 */

#include "MPU6050.h"
extern I2C_HandleTypeDef hi2c1;

void MPU6050_Init (void)
{
  uint8_t check, data;

  HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);  // read WHO_AM_I
  if (check == 104 || check == 112)  // 104 or 112 will be returned by the sensor if everything goes well
  {
	// power management register 0X6B we should write all 0's to wake the sensor up
	data = 0;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MAGT_1_REG, 1, &data, 1, 1000);

	// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
	data = 0x07;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 1000);

	// Set accelerometer configuration in ACCEL_CONFIG Register
	data = 0x00;  // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ± 2g
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 1000);

	// Set Gyroscopic configuration in GYRO_CONFIG Register
	data = 0x00;  // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ± 250 ̐/s
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 1000);
  }
}


void MPU6050_Read_Accel (float* Ax, float* Ay, float* Az)
{
	uint8_t Rec_Data[6];
	int16_t  Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;

	// Read 6 BYTES of data starting from ACCEL_XOUT_H (0x3B) register
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	*Ax = (float)Accel_X_RAW/16384.0;
	*Ay = (float)Accel_Y_RAW/16384.0;
	*Az = (float)Accel_Z_RAW/16384.0;
}

void MPU6050_Read_Gyro (float* Gx, float* Gy, float* Gz)
{
	uint8_t Rec_Data[6];
	int16_t  Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;

	// Read 6 BYTES of data starting from GYRO_XOUT_H register
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data [3]);
	Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data [5]);

	*Gx = (float)Gyro_X_RAW/131.0;
	*Gy = (float)Gyro_Y_RAW/131.0;
	*Gz = (float)Gyro_Z_RAW/131.0;
}




