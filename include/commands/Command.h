#ifndef _COMMAND_H
#define _COMMAND_H

#include "utils/Runnable.h"

class Command : Runnable
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
    Command(byte attachTo) : pin(attachTo) {}

    void switchState(boolean newState);
};

#endif