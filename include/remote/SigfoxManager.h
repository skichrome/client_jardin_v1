#ifndef _SIGFOXMANAGER_H
#define _SIGFOXMANAGER_H

#include <SigFox.h>

#include "utils/Runnable.h"

class SigfoxManager : Runnable
{
private:
    struct __attribute__((packed)) SigFoxData
    {
        uint32_t currentTimestamp;
        uint8_t luxValue;
        uint8_t altValue;
        uint8_t baroValue;
        uint8_t soilHumValue;
        uint8_t temperatureValue;
    } data;

    boolean isDataReady = false;

    void handleSigfoxResponseCallback();

protected:
    virtual void setup();
    virtual void loop();

public:
    SigfoxManager() {}

    /**
     * Valeurs à envoyer au serveur
     * - VEML7700 : lux
     * - MPL3115A2 : altitude, barometre, temperature
     * - Soil moisture
     * 
     * 1 octet en hexa : 255 valeurs possibles (FF)
     */
    void setDataToSend(uint32_t timestamp, uint8_t luxValue, uint8_t altValue, uint8_t baroValue, uint8_t soilHumValue, uint8_t temperatureValue);

    /**
     * Todo : 
     */
    long onConfigurationReceived();
};

#endif