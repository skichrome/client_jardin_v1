#include "sensors/SoilSensor.h"

SoilSensor::SoilSensor(Logger *mLogger, pin_size_t attachTo) : logger(NULL)
{
    logger = mLogger;
    pin = attachTo;
}

void SoilSensor::setup()
{
    pinMode(pin, INPUT);
    logger->e(F("Successfully configured Soil sensor"));
}

void SoilSensor::loop()
{
    switch (state)
    {
    case SoilSensor::READY:
    {
        startWaitCommunicationMs = millis();
        state = SoilSensor::WAIT_COMM;
        break;
    }
    case SoilSensor::WAIT_COMM:
        if (millis() - startWaitCommunicationMs > WAIT_COMMUNICATION_MS)
            state = SoilSensor::MEASURING;
        break;
    case SoilSensor::MEASURING:
    {
        humidity = analogRead(pin);

        String msg = "Raw Humidity: " + String(humidity);
        logger->e(msg);

        if (humidity > -1)
            state = SoilSensor::DONE;
    }
    break;
    case SoilSensor::DONE:
        break;
    default:
        break;
    }
}

boolean SoilSensor::isDataReady()
{
    return state == SoilSensor::DONE;
}

/**
 * Max dry value : ~895 (in air)
 * Min wet value : ~404 (100% water)
 */
void SoilSensor::updateSensorData(SensorsData *mData)
{
    if (humidity < 404)
        humidity = 404;

    if (humidity > 895)
        humidity = 895;

    mData->soilHumValue = map(humidity, 404, 895, 100, 0);
}

void SoilSensor::resetState()
{
    humidity = -1;
    state = SoilSensor::READY;
}