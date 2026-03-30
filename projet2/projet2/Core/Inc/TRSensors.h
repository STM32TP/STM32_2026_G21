/* TRSensors.h */
#ifndef __TR_SENSORS_H
#define __TR_SENSORS_H

#include "main.h"
#include "spi.h"

// Definición de pines para facilitar cambios futuros (ajusta si cambias de pin)
#define TR_CS_PORT    GPIOB
#define TR_CS_PIN     GPIO_PIN_6

// Número de sensores
#define NUM_SENSORS   5

// Prototipos de funciones
void TR_Sensors_Init(void);
uint16_t TR_TLC2543_ReadChannel(uint8_t channel);
void TR_Update_Sensors(uint16_t *sensor_array);

#endif /* __TR_SENSORS_H */
