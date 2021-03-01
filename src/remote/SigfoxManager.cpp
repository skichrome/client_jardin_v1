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
    if (isDataReady)
    {
        // Todo : convert data to hex, send data and wait callback
        SigFox.begin();
        SigFox.beginPacket();
        SigFox.write((uint8_t *)&data, sizeof(data));
        SigFox.endPacket(true);

        handleSigfoxResponseCallback();
        isDataReady = false;
    }
}

void SigfoxManager::setDataToSend(uint32_t timestamp, uint8_t luxValue, uint8_t altValue, uint8_t baroValue, uint8_t soilHumValue, uint8_t temperatureValue)
{
    data.currentTimestamp = timestamp;
    data.luxValue = luxValue;
    data.altValue = altValue;
    data.baroValue = baroValue;
    data.soilHumValue = soilHumValue;
    data.temperatureValue = temperatureValue;

    isDataReady = true;
}

void SigfoxManager::handleSigfoxResponseCallback()
{
    if (SigFox.parsePacket())
    {
        while (SigFox.available())
        {
            Serial.print("0x");
            Serial.println(SigFox.read(), HEX);
        }
    }
    else
    {
        Serial.println("No response from Sigfox backend");
    }
}