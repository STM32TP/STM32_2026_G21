/*
 * pcf8574.h
 *
 *  Created on: Mar 12, 2026
 *      Author: jaloz
 */

#ifndef __PCF8574_H
#define __PCF8574_H

#include "stm32l4xx_hal.h"

#define PCF8574_ADDR (0x20 << 1)

#define DSL_PIN 6
#define DSR_PIN 7

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t address;
    uint8_t port_state;

}PCF8574_HandleTypeDef;

void PCF8574_Init(PCF8574_HandleTypeDef *dev, I2C_HandleTypeDef *hi2c, uint8_t address);

HAL_StatusTypeDef PCF8574_ReadPort(PCF8574_HandleTypeDef *dev, uint8_t *data);

uint8_t PCF8574_ReadPin(PCF8574_HandleTypeDef *dev, uint8_t pin);

uint8_t PCF8574_ReadDSL(PCF8574_HandleTypeDef *dev);
uint8_t PCF8574_ReadDSR(PCF8574_HandleTypeDef *dev);

#endif
