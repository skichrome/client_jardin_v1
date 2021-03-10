#ifndef _SIGFOXMANAGER_H
#define _SIGFOXMANAGER_H

#include <SigFox.h>

#include "utils/Runnable.h"
#include "model/SensorsData.h"
#include "model/CallbackData.h"

class SigfoxManager : Runnable
{
private:
    SensorsData *sensorsData;
    CallbackData *callbackData;

    enum State
    {
        WAITING_DATA,
        SENDING,
        WAITING_CALLBACK,
        DONE,
        CALLBACK_ERROR
    } state;

    void handleSigfoxResponseCallback();

    /**
     * Todo : Update local timestamp and config
     */
    long onConfigurationReceived();

protected:
    virtual void setup();
    virtual void loop();

public:
    SigfoxManager(SensorsData *mData);

    boolean isDataSent();
    boolean isDataSentAndCallbackHandled();
};

#endif