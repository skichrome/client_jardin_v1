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
    switch (state)
    {
    case LuxSensor::WORKING:
        if (millis() - measuringDelayMs > MEASURING_DELAY_MS)
        {
            measuringDelayMs = millis();
            lux = sensor.readLux();
            if (lux != -1L)
            {
                state = LuxSensor::DONE;
                break;
            }
            led.blinkErrorCode(11);
        }
        break;
    case LuxSensor::DONE:
        if (millis() - measuringDelayMs > MEASURING_DELAY_MS)
        {
            Serial.print("Lux sensor measure: ");
            Serial.println(lux);
            measuringDelayMs = millis();
        }

        break;
    default:
        break;
    }
}