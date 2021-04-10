#include "sensors/SoilSensor.h"

SoilSensor::SoilSensor(Logger *mLogger, pin_size_t attachTo) : logger(NULL)
{
    logger = mLogger;
    pin = attachTo;
}

void SoilSensor::setup()
{
    pinMode(pin, INPUT);
}

void SoilSensor::loop()
{
    switch (state)
    {
    case SoilSensor::READY:
        state = SoilSensor::MEASURING;
        break;
    case SoilSensor::MEASURING:
    {
        humidity = analogRead(pin);

        String msg = "Humidity: " + String(humidity);
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

void SoilSensor::updateSensorData(SensorsData *mData)
{
    mData->soilHumValue = humidity;
}