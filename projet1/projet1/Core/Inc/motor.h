/*
 * motor.h
 *
 *  Created on: Feb 12, 2026
 *      Author:
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "main.h"

void Motor_Init(void);
void Motor_Left(int16_t speed);
void Motor_Right(int16_t speed);
void Motor_Stop(void);
void Moteur_Set(int gauche, int droite);

#endif
