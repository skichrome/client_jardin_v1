#include "sensors/BaroSensor.h"

BaroSensor::BaroSensor(Logger *mLogger) : logger(NULL)
{
    logger = mLogger;
}

void BaroSensor::setup()
{
    if (!sensor.begin())
    {
        logger->e("Can't start baro sensor communication");
        state = BaroSensor::NOT_FOUND;

        // Set to error values
        pascals = -2.0;
        altm = -2.0;
        temperature = -2.0;
    }
    else
        configureBaroSensor();

    measuringDelayMs = millis();
}

void BaroSensor::loop()
{
    if (millis() - measuringDelayMs > DELAY)
    {
        switch (state)
        {
        case BaroSensor::READY:
            state = BaroSensor::MEASURING;
            break;
        case BaroSensor::MEASURING:
        {
            pascals = sensor.getPressure();
            altm = sensor.getAltitude();
            temperature = sensor.getTemperature();

            String msg = "Pressure: " + String((uint8_t)map(pascals * 100, 8850000, 11410000, 0, 255)) + " computed altitude: " + String(altm) + " temperature: " + String(temperature);
            logger->e(msg);

            if (pascals > -1.0 && altm > -1.0 && temperature > -1.0)
                state = BaroSensor::DONE;
            break;
        }
        case BaroSensor::DONE:
            break;
        default:
            if (!sensor.begin())
                logger->e("Can't start baro sensor communication");
            else
                configureBaroSensor();
            break;
        }

        measuringDelayMs = millis();
    }
}

void BaroSensor::configureBaroSensor()
{
    state = BaroSensor::READY;
    logger->e("Successfully configured baro sensor");
}

boolean BaroSensor::isDataReady()
{
    return state == BaroSensor::DONE;
}

/**
 * Retrieve barometric sensors values into struct input and return this struct
 * 
 * Sensors data units : 
 * - Pressure : Pascals => ex: 96425.00 (float) : parse to int (multiply by 100 to avoid loosing float precision if necessary)
 *     and map from min 885 hPa to max 1141 hPa to 0->255 scale
 *     To get back to hPa just add value with min 885hPa
 */
void BaroSensor::updateSensorsData(SensorsData *mData)
{
    if (state == BaroSensor::DONE)
    {
        mData->altValue = altm;
        mData->baroValue = map(pascals * 100, 8850000, 11410000, 0, 255);
        mData->temperatureValue = (uint8_t)temperature * 100;
    }
}