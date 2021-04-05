#ifndef _LED_H
#define _LED_H

#include <Arduino.h>

class DebugLed
{
private:
    byte pin;

    enum LedState
    {
        ENABLED = HIGH,
        DISABLED = LOW
    } ledState = DISABLED;

    enum CycleState
    {
        WAITING,
        POWER_ON,
        POWER_OFF,
        PAUSING
    } cycleState = WAITING;

    unsigned long cycleCount = 0;

    const unsigned long HIGH_DURATION_MS = 250L;
    const unsigned long LOW_DURATION_MS = 500L;
    const unsigned long PAUSING_DURATION_MS = 1000L;

    unsigned long startDurationMs = 0L;

public:
    DebugLed(byte attachTo);
    
    void setup();
    void loop();

    void blinkErrorCode(unsigned int code);
};

#endif