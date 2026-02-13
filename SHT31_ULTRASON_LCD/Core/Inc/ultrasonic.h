
/*
 * ultrasonic.h
 *
 *  Created on: Feb 5, 2026
 *      Author: jaloz
 */


#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "stm32l4xx_hal.h"

void Ultrasonic_Init(void);
float Ultrasonic_Read(void);

#endif
