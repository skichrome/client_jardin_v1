#ifndef _SIGFOX_MANAGER_H
#define _SIGFOX_MANAGER_H

#include <RTCZero.h>
#include <SigFox.h>
#include <SD.h>

#include "utils/Logger.h"
#include "model/SensorsData.h"
#include "model/CallbackData.h"

#include "commands/SwitchCommand.h"

// #include "sensors/LuxSensor.h"
#include "sensors/BaroSensor.h"
#include "sensors/SoilSensor.h"

#define SENSORS_COMMAND_SW 3
#define MOISTURE_SENSOR_PIN A0

class SigfoxManager
{
private:
    Logger *logger;
    RTCZero *rtc;

    SensorsData sensorsData;

    SwitchCommand sensorsSwitch = SwitchCommand(logger, SENSORS_COMMAND_SW);

    // LuxSensor luxSensor = LuxSensor(logger);
    BaroSensor baroSensor = BaroSensor(logger);
    SoilSensor soilSensor = SoilSensor(logger, MOISTURE_SENSOR_PIN);

    const char configFileName[11] = "CONFIG.TXT";

    enum State
    {
        IDLE,
        WAITING_DATA,
        SENDING,
        WAITING_CALLBACK,
        DONE,
        CALLBACK_ERROR
    } state;

    void configureSDCard();
    boolean readSensorValues();
    void handleSigfoxResponseCallback();
    void saveCallbackToFile(CallbackData *callbackData);

public:
    SigfoxManager(Logger *mLogger, RTCZero *mRtc);

    void setup();
    void loop();

    boolean isDataSent();
    void resetState();
};

#endif