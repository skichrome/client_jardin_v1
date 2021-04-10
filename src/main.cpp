#include <Arduino.h>

#include <RTCZero.h>
#include <ArduinoLowPower.h>

/*
 * Auto indent in VSCode on Linux: ctrl + shift + i
 * Auto indent in VSCode on Windows: shift + alt + F ?
*/

#include "utils/Runnable.h"
#include "utils/Logger.h"
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

RTCZero rtc;

DebugLed led = DebugLed(LED_BUILTIN);
Logger logger = Logger(&rtc, &led);

SensorsData dataToSend = SensorsData();

SigfoxManager sfm = SigfoxManager(&logger, &dataToSend, &rtc);

LuxSensor luxSensor = LuxSensor(&logger);
BaroSensor baroSensor = BaroSensor(&logger);

SwitchCommand sensorsSwitch = SwitchCommand(&logger, SENSORS_COMMAND_SW);
RelayCommand sprinkle = RelayCommand(&logger, RELAY_ON_PIN, RELAY_OFF_PIN);

unsigned long startTimeMs = 0L;
unsigned int counter = 0;

void setup()
{
    rtc.begin();
    if (rtc.getEpoch() < 1609459200)
        rtc.setEpoch(1609459200); // 01/01/2021 00h00 is the reference time

    startTimeMs = millis();
    counter = 0;

    led.setup();
    logger.setup();
    Runnable::setupAll();

    delay(500);

    logger.e("Main setup done");
}

unsigned int count = 0;

void loop()
{
    led.loop();
    logger.loop();
    Runnable::loopAll();

    if (millis() - startTimeMs > 2000L && !sfm.isDataSent())
    {
        if (luxSensor.isDataReady() && baroSensor.isDataReady())
        {
            // Sensors are connected and have collected data, get values
            luxSensor.updateSensorData(&dataToSend);
            baroSensor.updateSensorsData(&dataToSend);

            // Todo : Query Soil moisture sensor and set timestamp
            //dataToSend.soilHumValue = 1;
            dataToSend.currentTimestamp = rtc.getEpoch();

            String timestampLog = "Timestamp to send: " + String(dataToSend.currentTimestamp);
            logger.e(timestampLog);

            // Measures are done, reset sensor switch
            sensorsSwitch.switchState(false);

            // Sprinkle relay Test
            // sprinkle.switchRelay();

            logger.e("All data fetched");
        }
        else
            logger.e("Lux or baro data not ready");

        startTimeMs = millis();
    }
}