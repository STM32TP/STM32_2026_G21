/*
 * motor.c
 *
 *  Created on: Feb 26, 2026
 *      Author: jaloz
 *
 *  Description :
 *  Ce fichier permet de contrôler deux moteurs à courant continu
 *  via un driver (type L298N ou similaire) en utilisant des signaux PWM.
 *  Il gère l'initialisation, l'arrêt et le contrôle de la vitesse
 *  et du sens de rotation de chaque moteur (gauche et droite).
 */

#include "motor.h"

// Handles des timers utilisés pour générer les signaux PWM
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

// Valeur maximale du PWM (détermine la vitesse maximale)
#define PWM_MAX 1000


/**
 * @brief Initialisation des moteurs
 *
 * Démarre les canaux PWM associés aux moteurs gauche et droit.
 * Chaque canal correspond à une sortie PWM connectée au driver moteur.
 */
void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // Moteur droit (PB4)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Moteur gauche (PB10)
}


/**
 * @brief Arrêt complet des moteurs
 *
 * Met le rapport cyclique (PWM) à 0 pour les deux moteurs,
 * ce qui entraîne leur arrêt immédiat.
 */
void Motor_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
}


/**
 * @brief Contrôle des moteurs gauche et droit
 *
 * Cette fonction permet de définir la vitesse et le sens de rotation
 * de chaque moteur.
 *
 * @param gauche  Valeur du moteur gauche (peut être négative pour marche arrière)
 * @param droite  Valeur du moteur droit (peut être négative pour marche arrière)
 *
 * Principe :
 * - Le signe détermine le sens de rotation
 * - La valeur absolue détermine la vitesse (PWM)
 */
void Moteur_Set(int gauche, int droite)
{
    // ========================
    // MOTEUR GAUCHE
    // ========================
    // PWMA → TIM2_CH3 → PB10

    if(gauche >= 0)
    {
        // Sens avant
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // AIN1
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // AIN2
    }
    else
    {
        // Sens arrière
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);

        // Conversion en valeur positive pour le PWM
        gauche = -gauche;
    }


    // ========================
    // MOTEUR DROIT
    // ========================
    // PWMB → TIM3_CH1 → PB4

    if(droite >= 0)
    {
        // Sens avant
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // BIN1
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // BIN2
    }
    else
    {
        // Sens arrière
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

        // Conversion en valeur positive pour le PWM
        droite = -droite;
    }


    // ========================
    // LIMITATION DU PWM
    // ========================
    // On s'assure que la valeur ne dépasse pas la valeur maximale autorisée

    if(gauche > PWM_MAX){
        gauche = PWM_MAX;
    }

    if(droite > PWM_MAX){
        droite = PWM_MAX;
    }


    // ========================
    // APPLICATION DU PWM
    // ========================
    // Mise à jour du rapport cyclique pour contrôler la vitesse

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, gauche); // Moteur gauche
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, droite); // Moteur droit
}
