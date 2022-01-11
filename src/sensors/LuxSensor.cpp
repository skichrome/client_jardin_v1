#include "sensors/LuxSensor.h"

LuxSensor::LuxSensor(Logger *mLogger) : logger(NULL)
{
    logger = mLogger;
}

void LuxSensor::setup()
{
    configureLuxSensor();
    measuringDelayMs = millis();
}

void LuxSensor::loop()
{
    if (millis() - measuringDelayMs > DELAY)
    {
        switch (state)
        {
        case LuxSensor::NOT_FOUND:
            configureLuxSensor();
            break;
        case LuxSensor::READY:
        {
            startWaitCommunicationMs = millis();
            state = LuxSensor::WAIT_COMM;
            break;
        }
        case LuxSensor::WAIT_COMM:
            if (millis() - startWaitCommunicationMs > WAIT_COMMUNICATION_MS)
            {
                if (!sensor.enabled())
                    sensor.enable(true);

                state = LuxSensor::MEASURING;
            }
            break;
        case LuxSensor::MEASURING:
        {
            lux = sensor.readLux();

            logger->e("Raw Lux: " + String(lux));

            if (lux > -1.0)
                state = LuxSensor::DONE;
            break;
        }
        case LuxSensor::DONE:
            break;
        }

        measuringDelayMs = millis();
    }
}

void LuxSensor::configureLuxSensor()
{
    if (!sensor.begin())
    {
        logger->e(F("Can't start lux sensor communication"));

        // Set to error values
        lux = -2.0;

        state = LuxSensor::NOT_FOUND;
    }
    else
    {
        sensor.setGain(VEML7700_GAIN_1);
        sensor.setIntegrationTime(VEML7700_IT_800MS);
        sensor.interruptEnable(false);

        state = LuxSensor::READY;
        logger->e(F("Successfully configured lux sensor"));
    }
}

boolean LuxSensor::isDataReady()
{
    return state == LuxSensor::DONE;
}

void LuxSensor::updateSensorData(SensorsData *mData)
{
    mData->luxValue = (int)lux;
}

void LuxSensor::resetState()
{
    if (sensor.enabled())
        sensor.enable(false);
    lux = -1.0;
    state = NOT_FOUND;
}