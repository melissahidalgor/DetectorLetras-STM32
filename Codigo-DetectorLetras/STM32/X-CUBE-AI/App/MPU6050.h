/*
 * MPU6050.h
 *
 *  Created on: Aug 29, 2025
 *      Author: Melissa
 *      I2C1 - 100 kHz , SCL: GPIOB6, SDA: GPIOB7
 */

#ifndef MPU6050_H_
#define MPU6050_H_

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#define MPU6050_ADDR        0xD0

#define SMPLRT_DIV_REG		0x19
#define GYRO_CONFIG_REG		0x1B
#define ACCEL_CONFIG_REG	0x1C
#define ACCEL_XOUT_H_REG	0x3B
#define TEMP_OUT_H_REG		0x41
#define GYRO_XOUT_H_REG		0x43
#define PWR_MAGT_1_REG		0x6B
#define WHO_AM_I_REG		0x75

void MPU6050_Init(void);
void MPU6050_Read_Accel (float* Ax, float* Ay, float* Az);
void MPU6050_Read_Gyro (float* Gx, float* Gy, float* Gz);

#define MPU6050_H_



#endif /* MPU6050_H_ */
