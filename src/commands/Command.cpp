#include "commands/Command.h"

void Command::setup()
{
    pinMode(pin, OUTPUT);
    currentState = Command::SWITCHED_ON;
    updatedState = currentState;
    digitalWrite(pin, currentState);
}

void Command::loop()
{
    if (currentState != updatedState)
    {
        currentState = updatedState;
        digitalWrite(pin, currentState);
    }
}

void Command::switchState(boolean newState)
{
    if (newState)
        updatedState = Command::SWITCHED_ON;
    else
        updatedState = Command::SWITCHED_OFF;
}