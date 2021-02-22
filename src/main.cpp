#include <Arduino.h>

/*
 * Auto indent in VSCode on Linux: ctrl + shift + i
 * Auto indent in VSCode on Windows: shift + alt + F ?
*/

#include "utils/Runnable.h"
#include "utils/DebugLed.h"

#include "sensors/LuxSensor.h"
#include "commands/Command.h"

#define SENSORS_COMMAND_SW 3

Runnable *Runnable::headRunnable = NULL;

DebugLed led = DebugLed(LED_BUILTIN);

Command sensorsSwitch = Command(SENSORS_COMMAND_SW);

//LuxSensor luxSensor = LuxSensor(led);

void setup()
{
    // Serial.begin(115200);
    Runnable::setupAll();
}

unsigned long startTimeMs = 5000L;
unsigned long count = 0L;

void loop()
{
    Runnable::loopAll();
}