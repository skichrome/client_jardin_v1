#include "sensors/LuxSensor.h"

void LuxSensor::setup()
{
    if (!sensor.begin())
    {
        Serial.println("Can't start lux sensor communication");
        led.blinkErrorCode(10);
        state = LuxSensor::NOT_FOUND;

        // Set to error values
        lux = -2L;
    }
    else
        configureLuxSensor();

    Serial.println("Successfully configured lux sensor");
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
            lux = sensor.readLux();
            
            Serial.print("Lux: ");
            Serial.println(lux);

            if (lux > -1L)
                state = LuxSensor::DONE;
            break;
        case LuxSensor::DONE:
            break;
        default:
            if (!sensor.begin())
                Serial.println("Can't start lux sensor communication");
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
}

boolean LuxSensor::isDataReady()
{
    return state == LuxSensor::DONE;
}

void LuxSensor::updateSensorData(SensorsData *mData)
{
    mData->luxValue = lux;
}