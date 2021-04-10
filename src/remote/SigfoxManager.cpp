#include "remote/SigfoxManager.h"

SigfoxManager::SigfoxManager(Logger *mLogger, SensorsData *mData, RTCZero *mRtc) : logger(NULL), sensorsData(NULL), rtc(NULL)
{
    logger = mLogger;
    sensorsData = mData;
    rtc = mRtc;
}

void SigfoxManager::setup()
{
    SigFox.begin();
    delay(100);
    SigFox.debug();
    SigFox.end();

    // SD Card configuration
    if (!SD.begin(4))
        logger->e("Can't configure SD Card lib in SigFoxManager");
    else
        logger->e("Successfully configured SigfoxManager");

    state = SigfoxManager::WAITING_DATA;
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
 * local_timestamp::uint:32:little-endian lux::uint:16:little-endian altitude::uint:8:little-endian baro::uint:8:little-endian soil_moisture::uint:8:little-endian temperature::uint:8:little-endian
 *
 * /!\ Numbers are written in little endian format /!\
 */
void SigfoxManager::loop()
{
    switch (state)
    {
    case SigfoxManager::WAITING_DATA:
        if (sensorsData->altValue && sensorsData->baroValue && sensorsData->currentTimestamp && sensorsData->luxValue && sensorsData->soilHumValue && sensorsData->temperatureValue)
            state = SigfoxManager::SENDING;
        break;

    case SigfoxManager::SENDING:
        // Todo : convert data to hex, send data and wait callback
        SigFox.begin();
        SigFox.beginPacket();
        SigFox.write((uint8_t *)sensorsData, sizeof(*sensorsData));
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

        callbackData = (CallbackData *)response;

        String msg = "Struct timestamp : " + String(callbackData->timestamp) + " Struct startTime : " + String(callbackData->sprinkleStartTime) + " Struct duration : " + String(callbackData->sprinleDuration);
        logger->e(msg);

        rtc->setEpoch(callbackData->timestamp);

        saveCallbackToFile();
    }
    else
    {
        logger->e("No response from Sigfox backend");
        state = SigfoxManager::CALLBACK_ERROR;
    }
}

void SigfoxManager::saveCallbackToFile()
{
    // Remove old config file before saving content
    if (SD.exists("config.txt"))
        SD.remove("config.txt");

    File callbackFile = SD.open("config.txt", FILE_WRITE);
    if (callbackFile)
    {
        callbackFile.println(callbackData->timestamp);
        callbackFile.println(callbackData->sprinkleStartTime);
        callbackFile.println(callbackData->sprinleDuration);

        callbackFile.close();
    }
    else
        logger->e("can't open config.txt to write callback data");

    File readTest = SD.open("config.txt", FILE_READ);

    if (readTest)
    {
        while (readTest.available())
        {
            String msg = "Data read : " + String(readTest.read());
            logger->e(msg);
        }
        readTest.close();
    }
    else
        logger->e("can't open config.txt for readTest");
}