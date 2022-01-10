#include <Arduino.h>

#include <RTCZero.h>
#include <ArduinoLowPower.h>

#include "utils/Logger.h"
#include "utils/DebugLed.h"

#include "commands/SprinkleCommand.h"
#include "remote/SigfoxManager.h"

#define DEBUG

// =================================
//              Fields
// =================================

RTCZero rtc;

DebugLed led = DebugLed(LED_BUILTIN);
Logger logger = Logger(&rtc, &led);

SigfoxManager sfm = SigfoxManager(&logger, &rtc);
SprinkleCommand sprinkle = SprinkleCommand(&logger, &rtc);

boolean sending = false;

// =================================
//              Methods
// =================================

void sleepToNextMeasure(int overridedDuration)
{
#ifdef DEBUG
    unsigned int durationMinutes = 2;
    unsigned int delayMs = 1000 * 60 * durationMinutes;

    unsigned long delayTime = millis();
    unsigned int count = 0;
    while (millis() - delayTime < delayMs)
    {
        delay(30000);
        logger.e("Sleeping in progress (" + String(++count) + "/" + String(durationMinutes * 2) + ")");
    }

    if (overridedDuration > 0)
    {
        while (millis() - overridedDuration < delayMs)
        {
            delay(30000);
            logger.e("Sleeping in progress (" + String(++count) + "/" + String((int)overridedDuration / 60 / 1000 * 2) + ")");
        }
    }
    else
    {
        while (millis() - delayTime < delayMs)
        {
            delay(30000);
            logger.e("Sleeping in progress (" + String(++count) + "/" + String(durationMinutes * 2) + ")");
        }
    }
#else
    if (overridedDuration > 0)
    {
        LowPower.deepSleep(overridedDuration);
    }
    else
    {
        int delayMs = 1000 * 60 * 15;
        LowPower.deepSleep(delayMs);
    }
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

    led.setup();
    logger.setup();
    sfm.setup();
    sprinkle.setup();

    delay(1000);

    logger.e(F("Main setup done"));
}

// ====== Loop ====== //

void loop()
{
    led.loop();
    logger.loop();
    sfm.loop();
    sprinkle.loop();

    bool isSprinkleRunning = false;
    int sprinkleDuration = sprinkle.checkIfSprinkleIsRequired();

    switch (sprinkleDuration)
    {
    case -1: // Sprinkle isn't required
    case -2: // Not in idle state
    case -3: // Last humidity value is under minimal humidity value to enable sprinkle
        break;

    default:
    {
        // Sprinkle is required
        sprinkle.requestStartSprinkle();
        isSprinkleRunning = true;
        break;
    }
    }

    if (isSprinkleRunning)
    {
        sleepToNextMeasure(sprinkleDuration);
        sprinkle.requestEndSprinkle();
        return;
    }

    if (sfm.isDataSent() || sfm.isDataSentAndCallbackHandled())
    {
        logger.e(F("OK, going to sleep"));
        sleepToNextMeasure(-4);
        sfm.resetState();
    }
}