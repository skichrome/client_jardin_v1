#include <Arduino.h>

#include <ArduinoLowPower.h>

/*
 * Auto indent in VSCode on Linux: ctrl + shift + i
 * Auto indent in VSCode on Windows: shift + alt + F ?
*/

#include "utils/Runnable.h"
#include "utils/DebugLed.h"

#include "sensors/LuxSensor.h"
#include "sensors/BaroSensor.h"
#include "commands/SwitchCommand.h"
#include "commands/RelayCommand.h"
#include "remote/SigfoxManager.h"
#include "model/SensorsData.h"

#define SENSORS_COMMAND_SW 3
#define RELAY_ON_PIN 1
#define RELAY_OFF_PIN 2

Runnable *Runnable::headRunnable = NULL;

DebugLed led = DebugLed(LED_BUILTIN);

SwitchCommand sensorsSwitch = SwitchCommand(SENSORS_COMMAND_SW);
RelayCommand sprinkle = RelayCommand(RELAY_ON_PIN, RELAY_OFF_PIN);

SensorsData dataToSend = SensorsData();

LuxSensor luxSensor = LuxSensor(led);
BaroSensor baroSensor = BaroSensor();
SigfoxManager sfm = SigfoxManager(&dataToSend);

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

    delay(500);

    Serial.println("Main setup done");
}

void loop()
{
    Runnable::loopAll();

    if (millis() - startTimeMs > 5000L && !sfm.isDataSent())
    {
        if (luxSensor.isDataReady() && baroSensor.isDataReady())
        {
            // Sensors are connected and have collected data, get values
            luxSensor.updateSensorData(&dataToSend);
            baroSensor.updateSensorsData(&dataToSend);

            // Todo : Query Soil moisture sensor and set timestamp
            dataToSend.soilHumValue = 1;
            dataToSend.currentTimestamp = millis();

            // Measures are done, reset sensor switch
            sensorsSwitch.switchState(false);

            // Sprinkle relay Test
            // sprinkle.switchRelay();

            Serial.println("Trigger");

            startTimeMs = millis();
        }
    }
}