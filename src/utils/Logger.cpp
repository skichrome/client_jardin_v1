#include "utils/Logger.h"

Logger::Logger(RTCZero *mRtc, DebugLed *mLed) : rtc(NULL), led(NULL)
{
    rtc = mRtc;
    led = mLed;
}

#ifdef DEBUG

void Logger::setup()
{
    logBufferPosition = 0;
    Serial.begin(115200);

    // Wait serial connexion
    while (!Serial)
    {
        led->loop();
        led->blinkErrorCode(2);
    }

    Serial.println("Logger ready");
}

void Logger::loop()
{
    if (logBufferPosition >= LOG_BUFFER_SIZE)
    {
        logBufferPosition = LOG_BUFFER_SIZE - 1;
        led->blinkErrorCode(3);
    }

    if (logBufferPosition != 0)
    {
        for (int i = 0; i < logBufferPosition; i++)
        {
            Serial.println(logBuffer[i]);
            logBuffer[i] = "-";
        }

        logBufferPosition = 0;
    }
}

#else

void Logger::setup()
{
    logBufferPosition = 0;
    while (!SD.begin(4))
    {
        led->loop();
        led->blinkErrorCode(2);
    }
}

void Logger::loop()
{
    if (logBufferPosition >= LOG_BUFFER_SIZE)
    {
        logBufferPosition = LOG_BUFFER_SIZE - 1;
        led->blinkErrorCode(3);
    }

    if (logBufferPosition != 0)
    {
        for (int i = 0; i < logBufferPosition; i++)
        {
            File logFile = SD.open("logs.txt", FILE_WRITE);

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

                logFile.println(logBuffer[i]);
                logBuffer[i] = "-";

                logFile.close();
            }
            else
                led->blinkErrorCode(3);
        }

        logBufferPosition = 0;
    }
}

#endif

void Logger::e(String msg)
{
    if (logBufferPosition >= LOG_BUFFER_SIZE)
    {
        led->blinkErrorCode(3);
        return;
    }

    logBuffer[logBufferPosition++] = msg;
}