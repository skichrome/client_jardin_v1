#include "sensors/BaroSensor.h"

void BaroSensor::setup()
{
    if (!sensor.begin())
        Serial.println("Can't start baro sensor communication");

    measuringDelayMs = millis();
    state = BaroSensor::READY;
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
            if (pascals == -1.0)
                pascals = sensor.getPressure();

            Serial.print("Pressure: ");
            Serial.println((uint8_t)map(pascals * 100, 8850000, 11410000, 0, 255));

            if (altm == -1.0)
                altm = sensor.getAltitude();

            Serial.print("computed altitude: ");
            Serial.println(altm);

            if (temperature == -1.0)
                temperature = sensor.getTemperature();

            Serial.print("temperature: ");
            Serial.println(temperature);

            if (pascals != -1.0 && altm != -1.0 && temperature != -1.0)
                state = BaroSensor::DONE;
            break;
        case BaroSensor::DONE:
            break;
        default:
            break;
        }

        measuringDelayMs = millis();
    }
}

/**
 * Retrieve barometric sensors values into struct input and return this struct
 * 
 * Sensors data units : 
 * - Pressure : Pascals => ex: 96425.00 (float) : parse to int (multiply by 100 to avoid loosing float precision if necessary)
 *     and map from min 885 hPa to max 1141 hPa to 0->255 scale
 *     To get back to hPa just add value with min 885hPa
 */
void BaroSensor::updateSensorsData(SensorsData &mData)
{
    if (state == BaroSensor::DONE)
    {
        mData.altValue = altm;
        mData.baroValue = map(pascals * 100, 8850000, 11410000, 0, 255);
        mData.temperatureValue = (uint8_t)temperature * 100;
    }
}