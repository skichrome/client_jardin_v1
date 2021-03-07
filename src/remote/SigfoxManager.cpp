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
    if (data.altValue && data.baroValue && data.currentTimestamp && data.luxValue && data.soilHumValue && data.temperatureValue)
    {
        // Todo : convert data to hex, send data and wait callback
        SigFox.begin();
        SigFox.beginPacket();
        SigFox.write((uint8_t *)&data, sizeof(data));
        SigFox.endPacket(true);

        handleSigfoxResponseCallback();
        // isDataReady = false;

        data.altValue = NULL;
        data.baroValue = NULL;
        data.currentTimestamp = NULL;
        data.luxValue = NULL;
        data.soilHumValue = NULL;
        data.temperatureValue = NULL;
    }
}

// void SigfoxManager::setDataToSend(SensorsData &mData)
// {
//     data = mData;

//     if (data.altValue && data.baroValue && data.currentTimestamp && data.luxValue && data.soilHumValue && data.temperatureValue)
//         isDataReady = true;
//     else
//         isDataReady = false;
// }

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