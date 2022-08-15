#ifndef _SPRINKLE_COMMAND_H
#define _SPRINKLE_COMMAND_H

#include <SD.h>
#include <RTCZero.h>

#include "utils/Logger.h"
#include "commands/RelayCommand.h"

#define RELAY_ON_PIN 1
#define RELAY_OFF_PIN 2

class SprinkleCommand
{
private:
    Logger *logger;
    RTCZero *rtc;

    RelayCommand relay = RelayCommand(logger, RELAY_ON_PIN, RELAY_OFF_PIN);

    uint8_t startTimeHour;
    uint8_t startTimeMin;
    uint8_t duration;
    uint8_t minimalSprinkleHumidity;
    uint8_t lastHumidityValue;

    const char configFileName[11] = "CONFIG.TXT";
    const char sprinkleHistoryFileName[13] = "SPRINKLE.TXT";
    const char lastSprinkleFileName[12] = "LAST_SP.TXT";

    enum State
    {
        NO_CONF,
        IDLE,
        SPRINKLE_START,
        SPRINKLE_RUN,
        SPRINKLE_END,
        SPRINKLE_DONE

    } state;

    void configureSDCard();
    void loadConfig();
    void startSprinkle();
    void endSprinkle();

public:
    SprinkleCommand(Logger *mLogger, RTCZero *mRtc);

    void setup();
    void loop();

    int checkIfSprinkleIsRequired();

    void requestStartSprinkle();
    void requestEndSprinkle();

    bool isSprinkleRunning();
    bool isSprinkleOff();
    void resetSprinkleState();
};

#endif