#ifndef _BARO_SENSOR_H
#define _BARO_SENSOR_H

#include <Adafruit_MPL3115A2.h>

#include "utils/Runnable.h"
#include "utils/Logger.h"
#include "model/SensorsData.h"

class BaroSensor : Runnable
{
private:
    Logger *logger;
    enum State
    {
        NOT_FOUND,
        READY,
        MEASURING,
        DONE
    } state;

    unsigned long measuringDelayMs = 0L;
    const unsigned long DELAY = 2000L;

    Adafruit_MPL3115A2 sensor = Adafruit_MPL3115A2();
    float pascals = -1.0;
    float altm = -1.0;
    float temperature = -100.0;

    void configureBaroSensor();

protected:
    virtual void setup();
    virtual void loop();

public:
    BaroSensor(Logger *mLogger);

    boolean isDataReady();
    void updateSensorsData(SensorsData *mData);
};

#endif