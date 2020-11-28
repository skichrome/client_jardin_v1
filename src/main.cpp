#include <Arduino.h>

/*
 * Auto indent in VSCode on Linux: ctrl + shift + i
 * Auto indent in VSCode on Windows: shift + alt + F ?
*/

#include "utils/Runnable.h"
#include "utils/DebugLed.h"

#include "sensors/LuxSensor.h"
#include "transmitter/WirelessManager.h"

#define CE_PIN 7
#define CSN_PIN 8

Runnable *Runnable::headRunnable = NULL;

DebugLed led = DebugLed(LED_BUILTIN_TX);

LuxSensor luxSensor = LuxSensor(led);

WirelessManager rfManager = WirelessManager(CE_PIN, CSN_PIN);

void setup()
{
    pinMode(LED_BUILTIN_RX, INPUT); // shut down rx led
    Serial.begin(115200);
    Runnable::setupAll();
}

void loop()
{
    Runnable::loopAll();
}