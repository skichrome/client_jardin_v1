#ifndef _SIGFOX_MANAGER_H
#define _SIGFOX_MANAGER_H

#include <RTCZero.h>
#include <SigFox.h>
#include <SD.h>

#include "utils/Logger.h"
#include "model/SensorsData.h"
#include "model/CallbackData.h"

class SigfoxManager
{
private:
    Logger *logger;
    SensorsData *sensorsData;
    RTCZero *rtc;

    const char configFileName[11] = "CONFIG.TXT";

    enum State
    {
        WAITING_DATA,
        SENDING,
        WAITING_CALLBACK,
        DONE,
        CALLBACK_ERROR
    } state;

    void configureSDCard();
    void handleSigfoxResponseCallback();
    void saveCallbackToFile(CallbackData *callbackData);

    long onConfigurationReceived();

public:
    SigfoxManager(Logger *mLogger, RTCZero *mRtc);

    void setup();
    void loop();

    boolean sendData(SensorsData *mSensorData);
    boolean isDataSent();
    boolean isDataSentAndCallbackHandled();
    void resetState();
};

#endif