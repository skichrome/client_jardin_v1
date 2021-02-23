#include "remote/SigfoxManager.h"

void SigfoxManager::setup()
{
    SigFox.begin();
    delay(100);
    SigFox.debug();
    SigFox.end();
}

void SigfoxManager::loop()
{
}

void SigfoxManager::setDataToSend(long luxValue, long altValue, long baroValue, float soilHumValue, float temperatureValue)
{
}