#include "commands/RelayCommand.h"

RelayCommand::RelayCommand(Logger *mLogger, byte attachToOn, byte attachToOff): logger(NULL)
{
    logger = mLogger;
    pinOn = attachToOn;
    pinOff = attachToOff;
}

void RelayCommand::setup()
{
    pinMode(pinOn, OUTPUT);
    pinMode(pinOff, OUTPUT);

    digitalWrite(pinOn, LOW);
    digitalWrite(pinOff, LOW);

    stateUpdateDelayMs = millis();
    currentRelayState = RelayCommand::IDLE_SET;

    logger->e("Successfully configured Relay");
}

void RelayCommand::loop()
{
    if (millis() - stateUpdateDelayMs > DELAY_MS)
    {
        switch (currentRelayState)
        {

        case RelayCommand::IDLE_SET:
            digitalWrite(pinOn, LOW);
            digitalWrite(pinOff, LOW);

            if (isSwitchingRequested)
            {
                currentRelayState = RelayCommand::WORKING_SET;
                isSwitchingRequested = false;
            }
            break;

        case RelayCommand::WORKING_SET:
            digitalWrite(pinOn, HIGH);
            digitalWrite(pinOff, LOW);
            currentRelayState = RelayCommand::IDLE_RESET;
            break;

        case RelayCommand::IDLE_RESET:
            digitalWrite(pinOn, LOW);
            digitalWrite(pinOff, LOW);

            if (isSwitchingRequested)
            {
                currentRelayState = RelayCommand::WORKING_RESET;
                isSwitchingRequested = false;
            }
            break;

        case RelayCommand::WORKING_RESET:
            digitalWrite(pinOn, LOW);
            digitalWrite(pinOff, HIGH);
            currentRelayState = RelayCommand::IDLE_SET;
            break;

        default:
            break;
        }

        stateUpdateDelayMs = millis();
    }
}

void RelayCommand::switchRelay()
{
    isSwitchingRequested = true;
}