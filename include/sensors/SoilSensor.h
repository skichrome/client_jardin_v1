#ifndef _SOIL_SENSOR_H
#define _SOIL_SENSOR_H

#include "utils/Logger.h"
#include "model/SensorsData.h"

class SoilSensor
{
private:
    Logger *logger;
    pin_size_t pin;

    enum State
    {
        READY,
        WAIT_COMM,
        MEASURING,
        DONE
    } state;

    unsigned long startWaitCommunicationMs = 0L;
    unsigned long WAIT_COMMUNICATION_MS = 1000L;

    int humidity = -1;

public:
    SoilSensor(Logger *mLogger, pin_size_t attachTo);

    void setup();
    void loop();

    boolean isDataReady();
    void updateSensorData(SensorsData *mData);
    void resetState();
};

#endif
