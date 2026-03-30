/*
 * ultrasonic.c
 *
 *  Created on: Feb 13, 2026
 *      Author: G45
 *
 *  Description :
 *  Ce fichier contient les fonctions nécessaires pour interfacer
 *  un capteur ultrasonique (type HC-SR04) avec un microcontrôleur.
 *  Il permet d'initialiser le capteur et de mesurer une distance
 *  en utilisant le temps de propagation d'une onde sonore.
 */

#include "ultrasonic.h"

// Définition du port et du pin pour le signal ECHO (réception)
static GPIO_TypeDef* usecho_gpio = GPIOA;
static uint16_t usecho_pin = GPIO_PIN_10;

// Définition du port et du pin pour le signal TRIGGER (émission)
static GPIO_TypeDef* ustrig_gpio = GPIOB;
static uint16_t ustrig_pin = GPIO_PIN_3;


/**
 * @brief Fonction interne pour générer un délai précis en microsecondes
 *
 * Cette fonction utilise le compteur de cycles du processeur (DWT->CYCCNT)
 * pour produire un délai très précis, nécessaire pour générer le signal TRIG.
 *
 * @param us Nombre de microsecondes à attendre
 */
static void delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;

    // Conversion du temps en nombre de cycles CPU
    uint32_t delayTicks = us * (HAL_RCC_GetHCLKFreq() / 1000000);

    // Attente active jusqu'à atteindre le nombre de cycles voulu
    while (DWT->CYCCNT - startTick < delayTicks);
}


/**
 * @brief Initialisation du capteur ultrasonique
 *
 * Cette fonction active le compteur de cycles DWT nécessaire pour les délais
 * précis et initialise l'état du pin TRIG.
 */
void Ultrasonic_Init(void) {

    // Activation du module de traçage (nécessaire pour DWT)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // Réinitialisation du compteur de cycles
    DWT->CYCCNT = 0;

    // Activation du compteur de cycles
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    // Mise à 0 du signal TRIG (état initial)
    HAL_GPIO_WritePin(ustrig_gpio, ustrig_pin, GPIO_PIN_RESET);

    // Attente pour stabiliser le capteur après l'initialisation
    HAL_Delay(50);
}


/**
 * @brief Lecture de la distance mesurée par le capteur ultrasonique
 *
 * Cette fonction envoie une impulsion TRIG, mesure la durée du signal ECHO
 * et calcule la distance correspondante.
 *
 * @return float Distance en centimètres.
 *         Retourne -1.0f en cas d'erreur (timeout ou mesure invalide).
 */
float Ultrasonic_Read(void) {

    uint32_t start = 0, stop = 0;

    // 1. Envoi de l'impulsion TRIGGER (durée de 10 µs)
    HAL_GPIO_WritePin(ustrig_gpio, ustrig_pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(ustrig_gpio, ustrig_pin, GPIO_PIN_RESET);

    // 2. Attente du front montant du signal ECHO (début de la mesure)
    uint32_t timeout = 100000; // Protection contre blocage

    while(HAL_GPIO_ReadPin(usecho_gpio, usecho_pin) == GPIO_PIN_RESET && timeout--);

    // Enregistrement du temps de départ
    start = DWT->CYCCNT;

    // 3. Attente du front descendant du signal ECHO (fin de la mesure)
    timeout = 90000;

    while(HAL_GPIO_ReadPin(usecho_gpio, usecho_pin) == GPIO_PIN_SET && timeout--);

    // Enregistrement du temps d'arrivée
    stop = DWT->CYCCNT;

    // Vérification d'erreur (timeout ou incohérence des temps)
    if(timeout == 0 || stop <= start) {
        return -1.0f;
    }

    // 4. Calcul du temps de propagation en microsecondes
    // temps (µs) = nombre de cycles / fréquence CPU (en MHz)
    float time_us = (float)(stop - start) / (HAL_RCC_GetHCLKFreq() / 1000000.0f);

    // Calcul de la distance :
    // Vitesse du son ≈ 0.0343 cm/µs
    // Division par 2 car l'onde fait un aller-retour

    return (time_us * 0.0343f) / 2.0f;
}
