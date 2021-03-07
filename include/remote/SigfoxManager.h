#ifndef _SIGFOXMANAGER_H
#define _SIGFOXMANAGER_H

#include <SigFox.h>

#include "utils/Runnable.h"
#include "model/SensorsData.h"

class SigfoxManager : Runnable
{
private:
    SensorsData &data;

    void handleSigfoxResponseCallback();

protected:
    virtual void setup();
    virtual void loop();

public:
    SigfoxManager(SensorsData &mData): data(mData) {}

    /**
     * Todo : Update local timestamp and config
     */
    long onConfigurationReceived();
};

#endif