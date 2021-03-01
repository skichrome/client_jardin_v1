#include <Arduino.h>

#include <ArduinoLowPower.h>

/*
 * Auto indent in VSCode on Linux: ctrl + shift + i
 * Auto indent in VSCode on Windows: shift + alt + F ?
*/

#include "utils/Runnable.h"
#include "utils/DebugLed.h"

#include "sensors/LuxSensor.h"
#include "commands/Command.h"
#include "remote/SigfoxManager.h"

#define SENSORS_COMMAND_SW 3

Runnable *Runnable::headRunnable = NULL;

DebugLed led = DebugLed(LED_BUILTIN);

Command sensorsSwitch = Command(SENSORS_COMMAND_SW);

LuxSensor luxSensor = LuxSensor(led);

SigfoxManager sfm = SigfoxManager();

unsigned long startTimeMs = 0L;
unsigned int counter = 0;

void setup()
{
    Serial.begin(115200);

    while (!Serial)
    {
    }

    startTimeMs = millis();
    counter = 0;

    Runnable::setupAll();
}

boolean measureStarted = false;
boolean measureDone = false;
boolean sigfoxDone = false;

void loop()
{
    Runnable::loopAll();

    if (millis() - startTimeMs > 10000L)
    {
        counter++;

        if (!measureDone && counter >= 1)
        {
            sensorsSwitch.switchState(true);

            Serial.print("Counter : ");
            Serial.println(counter);

            Serial.print("Final measured lux : ");
            Serial.println(luxSensor.getLuxMeasureAndStop());

            if (!measureDone && measureStarted)
            {
                sensorsSwitch.switchState(false);
                Serial.println("Reset sensors switch");
                measureDone = true;

                if (!sigfoxDone)
                {
                    /**
                     * ## SigFox Analysis
                     * Result on SigFox : dd6300006f647b5a78 => dd 63 00 00 | 6f | 64 | 7b | 5a | 78
                     * - Timestamp : dd630000 => 25565
                     * - Lux Sensor : 6f => 111
                     * - Altitude : 64 => 100
                     * - Baro : 7b => 123
                     * - Soil moisture : 5a => 90
                     * - Temperature : 78 => 120
                     * 
                     * /!\ Numbers are written in little endian format /!\
                     */
                    sfm.setDataToSend(25565, luxSensor.getLuxMeasureAndStop(), 100, 123, 90, 120);
                    sigfoxDone = true;
                }
            }
            measureStarted = true;
        }

        startTimeMs = millis();
    }
}