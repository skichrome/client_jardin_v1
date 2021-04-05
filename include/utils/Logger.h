#ifndef _LOG_H
#define _LOG_H

#include <SD.h>
#include <RTCZero.h>

#include "utils/DebugLed.h"

#define DEBUG
#define LOG_BUFFER_SIZE 10

class Logger
{
private:
    RTCZero *rtc;
    DebugLed *led;

    String logBuffer[LOG_BUFFER_SIZE];
    uint8_t logBufferPosition;

public:
    Logger(RTCZero *mRtc, DebugLed *mLed);

    void setup();
    void loop();

    void e(String msg);
};

#endif