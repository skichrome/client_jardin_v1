#ifndef _SENSORS_DATA_H
#define _SENSORS_DATA_H

#include <stdint.h>

/**
 * Valeurs à envoyer au serveur
 * - VEML7700 : lux
 * - MPL3115A2 : altitude, barometre, temperature
 * - Soil moisture
 * 
 * 1 octet en hexa : 255 valeurs possibles (FF)
 */
struct __attribute__((packed)) SensorsData
{
    uint8_t soilHumValue;
    uint16_t baroValue;
    uint16_t altValue;
    uint16_t temperatureValue;
    uint16_t luxValue;
};

#endif