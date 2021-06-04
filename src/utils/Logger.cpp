#include "utils/Logger.h"

Logger::Logger(RTCZero *mRtc, DebugLed *mLed) : rtc(NULL), led(NULL)
{
    rtc = mRtc;
    led = mLed;
}

#ifdef DEBUG

void Logger::setup()
{
    Serial.begin(115200);

    // Wait serial connexion
    while (!Serial)
    {
        led->loop();
        led->blinkErrorCode(2);
    }

    Serial.println(F("Logger ready"));
}

void Logger::loop()
{
}

#else

void Logger::setup()
{
    configureSDCard();
}

void Logger::loop()
{
}

#endif

void Logger::configureSDCard()
{
    while (!SD.begin(4))
    {
        led->loop();
        led->blinkErrorCode(2);
    }
}

void Logger::e(String msg)
{
#ifdef DEBUG
    Serial.println(msg);
#else
    configureSDCard();

    File logFile = SD.open("LOGS.TXT", FILE_WRITE);

    if (logFile)
    {
        logFile.print(rtc->getDay());
        logFile.print("/");
        logFile.print(rtc->getMonth());
        logFile.print("/");
        logFile.print(rtc->getYear());
        logFile.print("-");

        logFile.print(rtc->getHours());
        logFile.print(":");
        logFile.print(rtc->getMinutes());
        logFile.print(":");
        logFile.print(rtc->getSeconds());
        logFile.print("-");

        logFile.println(msg);

        logFile.close();
    }
    else
        led->blinkErrorCode(3);

    SD.end();
#endif
}