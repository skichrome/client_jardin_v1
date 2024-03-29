#include "sensors/BaroSensor.h"

BaroSensor::BaroSensor(Logger *mLogger) : logger(NULL)
{
    logger = mLogger;
    state = BaroSensor::READY;
}

void BaroSensor::setup()
{
    configureBaroSensor();
    measuringDelayMs = millis();
}

void BaroSensor::loop()
{
    if (millis() - measuringDelayMs > DELAY)
    {
        switch (state)
        {
        case BaroSensor::NOT_FOUND:
        {
            configureBaroSensor();
            break;
        }
        case BaroSensor::READY:
        {
            startWaitCommunicationMs = millis();
            state = BaroSensor::WAIT_COMM;
            break;
        }
        case BaroSensor::WAIT_COMM:
            if (millis() - startWaitCommunicationMs > WAIT_COMMUNICATION_MS)
                state = BaroSensor::MEASURING;
            break;
        case BaroSensor::MEASURING:
        {
            pascals = sensor.getPressure();
            logger->e("Raw Pressure: " + String(pascals));
            altm = sensor.getAltitude();
            logger->e("Raw computed altitude: " + String(altm));
            temperature = sensor.getTemperature();
            logger->e("Raw temperature: " + String(temperature));

            if (pascals > -1.0 && altm > -1.0 && temperature > -100.0)
                state = BaroSensor::DONE;
            break;
        }
        case BaroSensor::DONE:
            break;
        }

        measuringDelayMs = millis();
    }
}

void BaroSensor::configureBaroSensor()
{
    if (!sensor.begin())
    {
        logger->e(F("Can't start baro sensor communication"));

        // Set to error values
        pascals = -2.0;
        altm = -2.0;
        temperature = -200.0;

        state = BaroSensor::NOT_FOUND;
    }
    else
    {
        state = BaroSensor::READY;
        logger->e(F("Successfully configured baro sensor"));
    }
}

boolean BaroSensor::isDataReady()
{
    return state == BaroSensor::DONE;
}

/**
 * Retrieve barometric sensors values into struct input and return this struct
 * 
 * Sensors data units : 
 * - Altitude : Meters => range from 382m to 637 and mutliply by 100 to get max precision).
 * - Pressure : Pascals => ex: 96425.00 (float) : parse to int (multiply by 100 to avoid loosing float precision if necessary)
 *     and map from min 886 hPa to max 1141 hPa to 0->255 scale
 *     To get back to hPa just add value with min 885hPa
 * - Temperature : degrees => range from -50 to + 50 °C
 */
void BaroSensor::updateSensorsData(SensorsData *mData)
{
    if (state == BaroSensor::DONE)
    {
        mData->baroValue = abs(pascals) - 88600;
        mData->altValue = abs(altm * 100) - 38200;
        mData->temperatureValue = abs(temperature * 100);
    }
}

void BaroSensor::resetState()
{
    pascals = -1.0;
    altm = -1.0;
    temperature = -100.0;
    state = BaroSensor::NOT_FOUND;
}