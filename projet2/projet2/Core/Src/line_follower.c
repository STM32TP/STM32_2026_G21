/*
 * line_follower.c
 *
 * Description :
 * Ce module implémente un algorithme de suivi de ligne basé sur un
 * contrôleur PD (Proportionnel-Dérivé).
 * Il utilise un tableau de capteurs (typiquement 5 capteurs IR)
 * pour détecter la position de la ligne et ajuster la vitesse
 * des moteurs en conséquence.
 */

#include "line_follower.h"
#include "motor.h"

// Vitesse de base du robot (avance constante)
#define BASE_SPEED 200

// Gains du contrôleur PD
#define KP 35   // Gain proportionnel (réagit à l'erreur instantanée)
#define KD 120  // Gain dérivé (réagit à la variation de l'erreur)

// Seuil de détection de la ligne (à calibrer selon le capteur)
#define THRESHOLD 2400

// Distance cible (non utilisée ici mais peut servir pour extensions)
#define TARGET_DISTANCE 7


// Stocke l'erreur précédente (utile pour le terme dérivé)
static float last_error = 0;

/* ========================= */
/* INIT                      */
/* ========================= */
/**
 * @brief Initialisation du module de suivi de ligne
 *
 * Initialise les moteurs.
 */
void LineFollower_Init()
{
    Motor_Init();
}

/* ========================= */
/* LINE FOLLOW PD            */
/* ========================= */
/**
 * @brief Contrôle du suivi de ligne avec un régulateur PD
 *
 * @param s Tableau des valeurs des capteurs (5 capteurs)
 *
 * Principe :
 * - Convertit les valeurs des capteurs en "poids"
 * - Calcule la position de la ligne (centre de masse)
 * - Applique un contrôleur PD pour corriger la trajectoire
 */
void LineControl_PD(uint16_t *s)
{
    // Poids associés à chaque capteur (position relative)
    int weights[5] = {0,1000,2000,3000,4000};

    long weighted_sum = 0; // Somme pondérée
    long sum = 0;          // Somme totale des valeurs

    // ========================
    // TRAITEMENT DES CAPTEURS
    // ========================
    for(int i=0;i<5;i++)
    {
        // Inversion : plus la valeur est faible, plus on est sur la ligne
        int value = THRESHOLD - s[i];

        // Suppression des valeurs négatives
        if(value < 0){
            value = 0;
        }

        // Saturation pour éviter les valeurs trop élevées
        if(value > 1400){
            value = 1400;
        }

        // Calcul du centre de masse
        weighted_sum += (long)value * weights[i];
        sum += value;
    }

    // ========================
    // CAS : LIGNE PERDUE
    // ========================
    if(sum == 0)
    {
        // Si aucun capteur ne détecte la ligne,
        // on tourne dans la dernière direction connue
        if(last_error > 0){
            Moteur_Set(-200, 200); // Rotation à droite
        }else{
            Moteur_Set(200, -200); // Rotation à gauche
        }
        return;
    }

    // ========================
    // CALCUL DE POSITION
    // ========================
    // Position normalisée de la ligne (centre = 2000)
    int position = weighted_sum / sum;

    // Erreur par rapport au centre (normalisée)
    float error = (position - 2000) / 1000.0f;

    // ========================
    // CONTRÔLEUR PD
    // ========================
    float control = KP * error + KD * (error - last_error);

    // Mise à jour de l'erreur précédente
    last_error = error;

    // ========================
    // LIMITATION DU CONTRÔLE
    // ========================
    if(control > 400){
        control = 400;
    }
    if(control < -400){
        control = -400;
    }

    // ========================
    // CALCUL DES VITESSES
    // ========================
    int left  = BASE_SPEED + control;
    int right = BASE_SPEED - control;

    // Application aux moteurs
    // (le signe dépend du montage du robot)
    Moteur_Set(-left, -right);
}


/* ========================= */
/* MAIN UPDATE               */
/* ========================= */
/**
 * @brief Fonction principale appelée en boucle
 *
 * @param s Tableau des capteurs
 *
 * Cette fonction applique le contrôle PD à chaque itération.
 */
void LineFollower_Update(uint16_t *s)
{
    LineControl_PD(s);
}
