/*
 * capteurtemphumid.h
 *
 *  Created on: Feb 5, 2026
 *      Author: G45
 */

#ifndef SHT31_H
#define SHT31_H

#include "stm32l4xx_hal.h"

#define SHT31_ADDR  (0x44 << 1)

HAL_StatusTypeDef SHT31_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef SHT31_Read(I2C_HandleTypeDef *hi2c, float *temperature, float *humidity);

#endif

