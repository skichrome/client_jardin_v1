#ifndef _SOIL_SENSOR_H
#define _SOIL_SENSOR_H

#include "utils/Runnable.h"
#include "utils/Logger.h"
#include "model/SensorsData.h"

class SoilSensor : Runnable
{
private:
    Logger *logger;
    pin_size_t pin;

    int humidity = -1;

    enum State
    {
        READY,
        MEASURING,
        DONE
    } state;

protected:
    virtual void setup();
    virtual void loop();

public:
    SoilSensor(Logger *mLogger, pin_size_t attachTo);

    boolean isDataReady();
    void updateSensorData(SensorsData *mData);
};

#endif
