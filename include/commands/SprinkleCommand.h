#ifndef _SPRINKLE_COMMAND_H
#define _SPRINKLE_COMMAND_H

#include <SD.h>

#include "utils/Runnable.h"
#include "utils/Logger.h"
#include "commands/RelayCommand.h"

#define RELAY_ON_PIN 1
#define RELAY_OFF_PIN 2

class SprinkleCommand : Runnable
{
private:
    Logger *logger;
    RelayCommand relay = RelayCommand(logger, RELAY_ON_PIN, RELAY_OFF_PIN);

    uint8_t startTimeHour;
    uint8_t startTimeMin;
    uint8_t duration;
    
    const char configFileName[11] = "CONFIG.TXT";

    enum State
    {
        NO_CONF,
        IDLE,
        SPRINKLE_STARTED,
        SPRINKLE_DONE
    } state;

    void configureSDCard();
    void loadConfig();
    void sprinkle();

protected:
    virtual void setup();
    virtual void loop();

public:
    SprinkleCommand(Logger *mLogger);
};

#endif