#include "sensors/LuxSensor.h"

void LuxSensor::setup()
{
    while (!sensor.begin())
    {
        state = LuxSensor::NOT_FOUND;
        led.blinkErrorCode(10);
    }

    sensor.setGain(VEML7700_GAIN_1);
    sensor.setIntegrationTime(VEML7700_IT_800MS);
    sensor.interruptEnable(false);

    measuringDelayMs = millis();
    state = LuxSensor::WORKING;
}

void LuxSensor::loop()
{
    if (millis() - measuringDelayMs > DELAY)
    {
        switch (state)
        {
        case LuxSensor::WORKING:
            if (lux == -1L)
                lux = sensor.readLux();

            Serial.print("Lux sensor measure: ");
            Serial.println(lux);

            if (lux != -1L)
                state = LuxSensor::DONE;
            break;
        case LuxSensor::DONE:
            break;
        default:
            break;
        }

        measuringDelayMs = millis();
    }
}

void LuxSensor::updateSensorData(SensorsData &mData)
{
    mData.luxValue = lux;
}