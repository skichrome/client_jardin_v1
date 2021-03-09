#ifndef _SWITCH_COMMAND_H
#define _SWITCH_COMMAND_H

#include "utils/Runnable.h"

class SwitchCommand : Runnable
{
private:
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
    SwitchCommand(byte attachTo) : pin(attachTo) {}

    void switchState(boolean newState);
};

#endif