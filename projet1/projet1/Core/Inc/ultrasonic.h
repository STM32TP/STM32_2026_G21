/*
 * ultrasonic.h
 *
 *  Created on: Feb 13, 2026
 *      Author: G45
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "stm32l4xx_hal.h"

void Ultrasonic_Init(void);
float Ultrasonic_Read(void);

#endif

