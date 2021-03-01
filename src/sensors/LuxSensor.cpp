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
            
            Serial.print("Lux sensor measure: ");
            Serial.println(lux);
        }
        break;
    case LuxSensor::DONE:
        break;
    default:
        break;
    }
}

uint8_t LuxSensor::getLuxMeasureAndStop()
{
    state = LuxSensor::DONE;
    return lux;
}