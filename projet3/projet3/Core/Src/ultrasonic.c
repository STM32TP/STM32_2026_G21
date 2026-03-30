/*
 * ultrasonic.c
 *
 *  Created on: Mar 12, 2026
 *      Author: jaloz
 */

#include "ultrasonic.h"

static GPIO_TypeDef* usecho_gpio = GPIOA;//echo
static uint16_t usecho_pin = GPIO_PIN_10;
static GPIO_TypeDef* ustrig_gpio = GPIOB;//trig
static uint16_t ustrig_pin = GPIO_PIN_3;


// Fonction interne pour un dÃ©lai prÃ©cis en microsecondes
static void delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (HAL_RCC_GetHCLKFreq() / 1000000);
    while (DWT->CYCCNT - startTick < delayTicks);
}


void Ultrasonic_Init(void) {
    // Le compteur DWT doit Ãªtre activÃ© pour que delay_us fonctionne
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /*GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = us_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(us_gpio, &GPIO_InitStruct);*/

    HAL_GPIO_WritePin(ustrig_gpio, ustrig_pin, GPIO_PIN_RESET);
    HAL_Delay(50); // Attente stabilisation capteur
}


float Ultrasonic_Read(void) {
    uint32_t start = 0, stop = 0;


    // 1. Envoyer le Pulse Trigger (10us)

    HAL_GPIO_WritePin(ustrig_gpio, ustrig_pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(ustrig_gpio, ustrig_pin, GPIO_PIN_RESET);


    // 2. Attendre le front montant (dÃ©but de l'Ã©cho) avec timeout
    uint32_t timeout = 100000; // Timeout de sÃ©curitÃ©
    while(HAL_GPIO_ReadPin(usecho_gpio, usecho_pin) == GPIO_PIN_RESET && timeout--);
    start = DWT->CYCCNT;

    // 3. Attendre le front descendant (fin de l'Ã©cho)
    timeout = 90000;
    while(HAL_GPIO_ReadPin(usecho_gpio, usecho_pin) == GPIO_PIN_SET && timeout--);
    stop = DWT->CYCCNT;

    if(timeout == 0 || stop <= start){
    	return -1.0f;
    }

    // 4. Calcul de la distance
    // temps (us) = cycles / (FrÃ©quence CPU en MHz)
    float time_us = (float)(stop - start) / (HAL_RCC_GetHCLKFreq() / 1000000.0f);

    // Distance = (temps * vitesse du son 0.0343 cm/us) / 2
    return (time_us * 0.0343f) / 2.0f;
}

