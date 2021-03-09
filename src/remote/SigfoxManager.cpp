#include "remote/SigfoxManager.h"

SigfoxManager::SigfoxManager(SensorsData *mData) : data(NULL)
{
    data = mData;
}

void SigfoxManager::setup()
{
    SigFox.begin();
    delay(100);
    SigFox.debug();
    SigFox.end();

    state = SigfoxManager::WAITING_DATA;

    Serial.println("Successfully configured SigfoxManager");
}

/**
 * ## SigFox Analysis
 * Result on SigFox : dd6300006f647b5a78 => dd 63 00 00 | 6f | 64 | 7b | 5a | 78
 * - Timestamp : dd630000 => 25565
 * - Lux Sensor : 6f => 111
 * - Altitude : 64 => 100
 * - Baro : 7b => 123
 * - Soil moisture : 5a => 90
 * - Temperature : 78 => 120
 *
 * local_timestamp::uint:32 lux::uint:8 altitude::uint:8 baro::uint:8 soil_moisture::uint:8 temperature::uint:8
 *
 * /!\ Numbers are written in little endian format /!\
 */
void SigfoxManager::loop()
{
    switch (state)
    {
    case SigfoxManager::WAITING_DATA:
        if (data->altValue && data->baroValue && data->currentTimestamp && data->luxValue && data->soilHumValue && data->temperatureValue)
            state = SigfoxManager::SENDING;
        break;

    case SigfoxManager::SENDING:
        // Todo : convert data to hex, send data and wait callback
        SigFox.begin();
        SigFox.beginPacket();
        SigFox.write((uint8_t *)data, sizeof(data));
        SigFox.endPacket(true);
        state = SigfoxManager::WAITING_CALLBACK;
        break;

    case SigfoxManager::WAITING_CALLBACK:
        handleSigfoxResponseCallback();
        state = SigfoxManager::DONE;
        break;

    default:
        break;
    }
}

boolean SigfoxManager::isDataSent()
{
    return state == SigfoxManager::DONE || state == SigfoxManager::CALLBACK_ERROR;
}

boolean SigfoxManager::isDataSentAndCallbackHandled()
{
    return state == SigfoxManager::DONE;
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
        state = SigfoxManager::CALLBACK_ERROR;
    }
}