#ifndef _LUX_SENSOR_H
#define _LUX_SENSOR_H

#include <Arduino.h>
#include "utils/Runnable.h"
#include "utils/Logger.h"
#include "utils/DebugLed.h"
#include "model/SensorsData.h"

#include <Adafruit_VEML7700.h>

class LuxSensor : public Runnable
{
private:
    Logger *logger;

    enum State
    {
        NOT_FOUND,
        READY,
        WAIT_COMM,
        MEASURING,
        DONE
    } state = NOT_FOUND;

    Adafruit_VEML7700 sensor = Adafruit_VEML7700();

    float lux = -1.0;

    unsigned long startWaitCommunicationMs = 0L;
    unsigned long WAIT_COMMUNICATION_MS = 1000L;

    unsigned long measuringDelayMs = 0L;
    const unsigned long DELAY = 2000L;

    void configureLuxSensor();

protected:
    virtual void setup();
    virtual void loop();

public:
    LuxSensor(Logger *mLogger);

    boolean isDataReady();
    void updateSensorData(SensorsData *mData);
    void resetState();
};

#endif