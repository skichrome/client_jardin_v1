#ifndef _BAROSENSOR_H
#define _BAROSENSOR_H

#include <Adafruit_MPL3115A2.h>

#include "utils/Runnable.h"
#include "model/SensorsData.h"

class BaroSensor : Runnable
{
private:
    enum State
    {
        READY,
        MEASURING,
        DONE
    } state;

    unsigned long measuringDelayMs = 0L;
    const unsigned long DELAY = 2000L;

    Adafruit_MPL3115A2 sensor = Adafruit_MPL3115A2();
    float pascals = -1.0;
    float altm = -1.0;
    float temperature = -1.0;

protected:
    virtual void setup();
    virtual void loop();

public:
    BaroSensor() {}
    void updateSensorsData(SensorsData &mData);
};

#endif