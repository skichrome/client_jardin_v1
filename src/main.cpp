#include <Arduino.h>

/*
 * Auto indent in VSCode on Linux: ctrl + shift + i
 * Auto indent in VSCode on Windows: shift + alt + F ?
*/

#include "utils/Runnable.h"
#include "utils/DebugLed.h"

Runnable* Runnable::headRunnable = NULL;

DebugLed led = DebugLed(LED_BUILTIN_TX);

void setup()
{
    pinMode(LED_BUILTIN_RX, INPUT);

    Runnable::setupAll();
}

unsigned long blinkInterval = 5000L;

void loop()
{
    Runnable::loopAll();

    if (millis() - blinkInterval > 10000L)
    {
        led.blinkErrorCode(5);
        blinkInterval = millis();
    }
    
}