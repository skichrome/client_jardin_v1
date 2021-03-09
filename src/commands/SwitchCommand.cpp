#include "commands/SwitchCommand.h"

void SwitchCommand::setup()
{
    pinMode(pin, OUTPUT);
    currentState = SwitchCommand::SWITCHED_ON;
    updatedState = currentState;
    digitalWrite(pin, currentState);
    
    Serial.println("Successfully configured sensor switch");
}

void SwitchCommand::loop()
{
    if (currentState != updatedState)
    {
        currentState = updatedState;
        digitalWrite(pin, currentState);
    }
}

void SwitchCommand::switchState(boolean newState)
{
    if (newState)
        updatedState = SwitchCommand::SWITCHED_ON;
    else
        updatedState = SwitchCommand::SWITCHED_OFF;
}