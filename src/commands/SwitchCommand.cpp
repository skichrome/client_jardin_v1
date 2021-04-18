#include "commands/SwitchCommand.h"

SwitchCommand::SwitchCommand(Logger *mLogger, byte attachTo) : logger(NULL)
{
    logger = mLogger;
    pin = attachTo;
}

void SwitchCommand::setup()
{
    pinMode(pin, OUTPUT);
    currentState = SwitchCommand::SWITCHED_OFF;
    updatedState = currentState;
    digitalWrite(pin, currentState);

    logger->e("Successfully configured sensor switch");
}

void SwitchCommand::loop()
{
    if (currentState != updatedState)
    {
        currentState = updatedState;
        digitalWrite(pin, currentState);
        logger->e("Updated sensor state");
    }
}

void SwitchCommand::switchState(boolean newState)
{
    if (newState)
        updatedState = SwitchCommand::SWITCHED_ON;
    else
        updatedState = SwitchCommand::SWITCHED_OFF;
}

boolean SwitchCommand::isSwitchedOff()
{
    return currentState == SwitchCommand::SWITCHED_OFF;
}