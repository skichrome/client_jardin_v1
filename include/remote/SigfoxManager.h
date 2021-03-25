#ifndef _SIGFOXMANAGER_H
#define _SIGFOXMANAGER_H

#include <RTCZero.h>
#include <SigFox.h>
#include <SD.h>

#include "utils/Runnable.h"
#include "model/SensorsData.h"
#include "model/CallbackData.h"

class SigfoxManager : Runnable
{
private:
    SensorsData *sensorsData;
    CallbackData *callbackData;
    RTCZero *rtc;

    enum State
    {
        WAITING_DATA,
        SENDING,
        WAITING_CALLBACK,
        DONE,
        CALLBACK_ERROR
    } state;

    void handleSigfoxResponseCallback();
    void saveCallbackToFile();

    long onConfigurationReceived();

protected:
    virtual void setup();
    virtual void loop();

public:
    SigfoxManager(SensorsData *mData, RTCZero *mRtc);

    boolean isDataSent();
    boolean isDataSentAndCallbackHandled();
};

#endif