#ifndef _SWITCH_COMMAND_H
#define _SWITCH_COMMAND_H

#include "utils/Runnable.h"
#include "utils/Logger.h"

class SwitchCommand : Runnable
{
private:
    Logger *logger;
    byte pin;
    enum State
    {
        SWITCHED_ON = LOW,
        SWITCHED_OFF = HIGH
    } currentState;

    State updatedState;

protected:
    virtual void setup();
    virtual void loop();

public:
    SwitchCommand(Logger *mLogger, byte attachTo);

    void switchState(boolean newState);
    boolean isSwitchedOff();
};

#endif