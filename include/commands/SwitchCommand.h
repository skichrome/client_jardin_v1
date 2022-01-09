#ifndef _SWITCH_COMMAND_H
#define _SWITCH_COMMAND_H

#include "utils/Logger.h"

class SwitchCommand
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

public:
    SwitchCommand(Logger *mLogger, byte attachTo);

    void setup();
    void loop();

    void switchState(boolean newState);
    boolean isSwitchedOff();
};

#endif