#ifndef _LUX_SENSOR_H
#define _LUX_SENSOR_H

#include <Arduino.h>
#include "utils/Runnable.h"
#include "utils/DebugLed.h"

#include "Adafruit_VEML7700.h"

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
    Adafruit_VEML7700 sensor;

    long lux = -1L;

    unsigned long measuringDelayMs = 0L;
    const unsigned long MEASURING_DELAY_MS = 2000L;

protected:
    virtual void setup();
    virtual void loop();

public:
    LuxSensor(DebugLed &mLed) : led(mLed),
                                sensor(Adafruit_VEML7700())
    {
    }
};

#endif