#include "remote/SigfoxManager.h"

SigfoxManager::SigfoxManager(Logger *mLogger, RTCZero *mRtc) : logger(NULL)
{
    logger = mLogger;
    rtc = mRtc;

    state = SigfoxManager::WAITING_DATA;
}

void SigfoxManager::setup()
{
    SigFox.begin();
    delay(100);
    SigFox.debug();
    SigFox.end();

    sensorsSwitch.setup();
    luxSensor.setup();
    baroSensor.setup();
    soilSensor.setup();

    configureSDCard();
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
 * Sigfox Payload config (in callback editor : device type -> name -> callbacks -> edit)
 * soil_moisture::uint:8:little-endian baro::uint:16:little-endian alt::uint:16:little-endian temp::uint:16:little-endian lux::uint:16:little-endian
 *
 * /!\ Numbers are written in little endian format /!\
 */
void SigfoxManager::loop()
{
    sensorsSwitch.loop();
    luxSensor.loop();
    baroSensor.loop();
    soilSensor.loop();

    switch (state)
    {
    case SigfoxManager::WAITING_DATA:
    {
        if (readSensorValues())
        {
            state = SigfoxManager::SENDING;
        }
        break;
    }
    case SigfoxManager::SENDING:
    {
        logger->e(F("SigfoxManager has received data to send"));
        logger->e("(soilMoisture: " + String(sensorsData.soilHumValue) + ")-(baro: " + String(sensorsData.baroValue) + ")-(alt: " + String(sensorsData.altValue) + ")-(temp: " + String(sensorsData.temperatureValue) + ")-(lux: " + String(sensorsData.luxValue) + ")");
        SigFox.begin();
        SigFox.beginPacket();
        SigFox.write((uint8_t *)&sensorsData, sizeof(sensorsData));
        SigFox.endPacket(true);
        state = SigfoxManager::WAITING_CALLBACK;
        break;
    }
    case SigfoxManager::WAITING_CALLBACK:
    {
        handleSigfoxResponseCallback();
        break;
    }
    case SigfoxManager::CALLBACK_ERROR:
    case SigfoxManager::DONE:
    {
        sensorsData = {};
        sensorsSwitch.switchState(false);
        state = SigfoxManager::WAITING_DATA;
        break;
    }
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

void SigfoxManager::resetState()
{
    luxSensor.resetState();
    baroSensor.resetState();
    soilSensor.resetState();

    sensorsData = {};
    sensorsSwitch.switchState(false);
    state = SigfoxManager::WAITING_DATA;
}

void SigfoxManager::configureSDCard()
{
    if (!SD.begin(4))
        logger->e(F("Can't configure SD Card lib in SigFoxManager"));
    else
        logger->e(F("Successfully configured SigfoxManager"));
}

boolean SigfoxManager::readSensorValues()
{
    // To execute next part of code sensor sw must be on (case all sensors data OK but before sensors switch turn off)
    if (sensorsSwitch.isSwitchedOff())
    {
        logger->e(F("Turning on sensors switch"));
        sensorsSwitch.switchState(true);
    }

    if (luxSensor.isDataReady() && baroSensor.isDataReady() && soilSensor.isDataReady())
    {
        // Sensors are connected and have collected data, get values
        luxSensor.updateSensorData(&sensorsData);
        baroSensor.updateSensorsData(&sensorsData);
        soilSensor.updateSensorData(&sensorsData);

        // Measures are done, reset sensor switch
        sensorsSwitch.switchState(false);
        return true;
    }

    return false;
}

void SigfoxManager::handleSigfoxResponseCallback()
{
    if (SigFox.parsePacket())
    {
        // Position in callback response
        int counter = 0;
        // Store callback response, length is always 8
        uint8_t response[8];

        // Populate response array while data available
        while (SigFox.available())
        {
            uint8_t inputByte = SigFox.read();
            response[counter++] = inputByte;
        }

        CallbackData *callbackData = (CallbackData *)response;

        logger->e(F("Callback data has been received"));

        rtc->setEpoch(callbackData->timestamp);

        saveCallbackToFile(callbackData);
        state = SigfoxManager::DONE;
    }
    else
    {
        logger->e(F("No response from Sigfox backend"));
        state = SigfoxManager::CALLBACK_ERROR;
    }
}

void SigfoxManager::saveCallbackToFile(CallbackData *callbackData)
{
    configureSDCard();

    // Remove old config file before saving content
    if (SD.exists(configFileName))
        SD.remove(configFileName);
    else
        logger->e(F("CONFIX.TXT don't exists"));

    File callbackFile = SD.open(configFileName, FILE_WRITE);
    if (callbackFile)
    {
        callbackFile.println(callbackData->timestamp);
        callbackFile.println(callbackData->sprinkleStartHour);
        callbackFile.println(callbackData->sprinkleStartMinute);
        callbackFile.println(callbackData->sprinleDuration);
        callbackFile.println(callbackData->minimalSprinkleHumidity);
        callbackFile.println(sensorsData.soilHumValue);

        callbackFile.close();
    }
    else
        logger->e(F("can't open CONFIG.TXT to write callback data"));
}