#include <Arduino.h>

#include <RTCZero.h>
#include <ArduinoLowPower.h>

#include "utils/Logger.h"
#include "utils/DebugLed.h"

#include "commands/SwitchCommand.h"
#include "commands/RelayCommand.h"
#include "remote/SigfoxManager.h"

#define DEBUG

// =================================
//              Fields
// =================================

RTCZero rtc;

DebugLed led = DebugLed(LED_BUILTIN);
Logger logger = Logger(&rtc, &led);

SigfoxManager sfm = SigfoxManager(&logger, &rtc);

unsigned long startTimeMs = 0L;
boolean sending = false;

// =================================
//              Methods
// =================================

void sleepToNextMeasure()
{
#ifdef DEBUG
    unsigned int durationMinutes = 2;
    unsigned int delayMs = 1000 * 60 * durationMinutes;

    unsigned long delayTime = millis();
    unsigned int count = 0;
    while (millis() - delayTime < delayMs)
    {
        delay(30000);
        logger.e("Sleeping in progress (" + String(++count) + "/" + String(durationMinutes) + ")");
    }
#else
    unsigned int delayMs = 1000 * 60 * 15;
    LowPower.deepSleep(delayMs);
#endif
    logger.e("Wake up");
}

// =================================
//     Arduino required Methods
// =================================

// ====== Setup ====== //

void setup()
{
    rtc.begin();
    if (rtc.getEpoch() < 1609459200)
        rtc.setEpoch(1609459200); // 01/01/2021 00h00 is the reference time

    startTimeMs = millis();

    led.setup();
    logger.setup();
    sfm.loop();

    delay(1000);

    logger.e(F("Main setup done"));
}

// ====== Loop ====== //

void loop()
{
    led.loop();
    logger.loop();
    sfm.loop();

    if(sfm.isDataSent() || sfm.isDataSentAndCallbackHandled())
    {
        sleepToNextMeasure();
        sfm.resetState();
    }
}