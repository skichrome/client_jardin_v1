#ifndef _LUX_SENSOR_H
#define _LUX_SENSOR_H

#include <Arduino.h>
#include "utils/Runnable.h"
#include "utils/DebugLed.h"
#include "model/SensorsData.h"

#include <Adafruit_VEML7700.h>

class LuxSensor : public Runnable
{
private:
    enum State
    {
        NOT_FOUND,
        WORKING,
        DONE
    } state = NOT_FOUND;

    DebugLed &led;
    Adafruit_VEML7700 sensor = Adafruit_VEML7700();

    long lux = -1L;

    unsigned long measuringDelayMs = 0L;
    const unsigned long DELAY = 2000L;

protected:
    virtual void setup();
    virtual void loop();

public:
    LuxSensor(DebugLed &mLed) : led(mLed) {}

    void updateSensorData(SensorsData &mData);
};

#endif