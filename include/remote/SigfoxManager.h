#ifndef _SIGFOXMANAGER_H
#define _SIGFOXMANAGER_H

#include <SigFox.h>

#include "utils/Runnable.h"

class SigfoxManager : Runnable
{
private:
protected:
    virtual void setup();
    virtual void loop();

public:
    SigfoxManager();

    /**
     * Valeurs à envoyer au serveur
     * - VEML7700 : lux
     * - MPL3115A2 : altitude, barometre, temperature
     * - Soil moisture
     */ 
    void setDataToSend(long luxValue, long altValue, long baroValue, float soilHumValue, float temperatureValue);

    /**
     * Todo : 
     */
    long onConfigurationReceived();
};

#endif