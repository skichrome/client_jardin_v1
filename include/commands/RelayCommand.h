#ifndef _RELAY_COMMAND_H
#define _RELAY_COMMAND_H

#include "utils/Runnable.h"

class RelayCommand : Runnable
{
private:
    byte pinOn, pinOff;

    enum State
    {
        WORKING_SET,
        IDLE_SET,
        WORKING_RESET,
        IDLE_RESET
    } currentRelayState;

    boolean isSwitchingRequested = false;

    unsigned long stateUpdateDelayMs = 0L;
    const unsigned long DELAY_MS = 200L;

protected:
    virtual void setup();
    virtual void loop();

public:
    RelayCommand(byte attachToOn, byte attachToOff);
    void switchRelay();
};

#endif