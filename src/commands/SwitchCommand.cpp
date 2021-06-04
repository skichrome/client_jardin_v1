#include "commands/SwitchCommand.h"

SwitchCommand::SwitchCommand(Logger *mLogger, byte attachTo) : logger(NULL)
{
    logger = mLogger;
    pin = attachTo;
    currentState = SwitchCommand::SWITCHED_OFF;
    updatedState = currentState;
}

void SwitchCommand::setup()
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, currentState);

    logger->e(F("Successfully configured sensor switch"));
}

void SwitchCommand::loop()
{
    if (currentState != updatedState)
    {
        currentState = updatedState;
        digitalWrite(pin, currentState);
        logger->e(F("Updated sensor state"));
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