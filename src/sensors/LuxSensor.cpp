#include "sensors/LuxSensor.h"

LuxSensor::LuxSensor(Logger *mLogger) : logger(NULL)
{
    logger = mLogger;
}

void LuxSensor::setup()
{
    if (!sensor.begin())
    {
        logger->e("Can't start lux sensor communication");
        state = LuxSensor::NOT_FOUND;

        // Set to error values
        lux = -2.0;
    }
    else
        configureLuxSensor();
}

void LuxSensor::loop()
{
    if (millis() - measuringDelayMs > DELAY)
    {
        switch (state)
        {
        case LuxSensor::READY:
            state = LuxSensor::MEASURING;
            break;
        case LuxSensor::MEASURING:
        {
            lux = sensor.readLux();

            String msg = "Raw Lux: " + String(lux);
            logger->e(msg);

            if (lux > -1.0)
                state = LuxSensor::DONE;
            break;
        }
        case LuxSensor::DONE:
            break;
        default:
            if (!sensor.begin())
                logger->e("Can't start lux sensor communication");
            else
                configureLuxSensor();
            break;
        }

        measuringDelayMs = millis();
    }
}

void LuxSensor::configureLuxSensor()
{
    sensor.setGain(VEML7700_GAIN_1);
    sensor.setIntegrationTime(VEML7700_IT_800MS);
    sensor.interruptEnable(false);

    measuringDelayMs = millis();
    state = LuxSensor::READY;
    logger->e("Successfully configured lux sensor");
}

boolean LuxSensor::isDataReady()
{
    return state == LuxSensor::DONE;
}

void LuxSensor::updateSensorData(SensorsData *mData)
{
    mData->luxValue = lux;
}

void LuxSensor::resetState()
{
    lux = -1.0;
    state = NOT_FOUND;
}