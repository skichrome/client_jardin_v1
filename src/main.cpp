#include <Arduino.h>

#include <RTCZero.h>
#include <ArduinoLowPower.h>

#include "utils/Runnable.h"
#include "utils/Logger.h"
#include "utils/DebugLed.h"

#include "sensors/LuxSensor.h"
#include "sensors/BaroSensor.h"
#include "sensors/SoilSensor.h"
#include "commands/SwitchCommand.h"
#include "commands/RelayCommand.h"
#include "remote/SigfoxManager.h"
#include "model/SensorsData.h"

#define SENSORS_COMMAND_SW 3
#define RELAY_ON_PIN 1
#define RELAY_OFF_PIN 2
#define MOISTURE_SENSOR_PIN A0

Runnable *Runnable::headRunnable = NULL;

RTCZero rtc;

DebugLed led = DebugLed(LED_BUILTIN);
Logger logger = Logger(&rtc, &led);

SensorsData dataToSend = SensorsData();

SigfoxManager sfm = SigfoxManager(&logger, &dataToSend, &rtc);

LuxSensor luxSensor = LuxSensor(&logger);
BaroSensor baroSensor = BaroSensor(&logger);
SoilSensor soilSensor = SoilSensor(&logger, MOISTURE_SENSOR_PIN);

SwitchCommand sensorsSwitch = SwitchCommand(&logger, SENSORS_COMMAND_SW);
RelayCommand sprinkle = RelayCommand(&logger, RELAY_ON_PIN, RELAY_OFF_PIN);

unsigned long startTimeMs = 0L;
boolean sending = false;

void setup()
{
    rtc.begin();
    if (rtc.getEpoch() < 1609459200)
        rtc.setEpoch(1609459200); // 01/01/2021 00h00 is the reference time

    startTimeMs = millis();

    led.setup();
    logger.setup();
    Runnable::setupAll();

    delay(1000);

    logger.e("Main setup done");
}

void loop()
{
    led.loop();
    logger.loop();
    Runnable::loopAll();

    // To execute next part of code sensor sw must be on (case all sensors data OK but before sensors switch turn off)
    if (!sending && sensorsSwitch.isSwitchedOff())
    {
        logger.e("Bad sensor switch state, waiting next loop execution");
        sensorsSwitch.switchState(true);
        return;
    }

    if (millis() - startTimeMs > 1000L)
    {
        if (luxSensor.isDataReady() && baroSensor.isDataReady() && soilSensor.isDataReady())
        {
            // Sensors are connected and have collected data, get values
            luxSensor.updateSensorData(&dataToSend);
            baroSensor.updateSensorsData(&dataToSend);
            soilSensor.updateSensorData(&dataToSend);

            // Todo : Set timestamp will trigger SigFox upload
            dataToSend.currentTimestamp = rtc.getEpoch();

            // Measures are done, reset sensor switch
            sensorsSwitch.switchState(false);

            // Sprinkle relay Test
            // sprinkle.switchRelay();

            logger.e("All data fetched");
            sending = true;

            if (sfm.isDataSent() && sensorsSwitch.isSwitchedOff())
            {
                // Reset and sleep (using delay if debug avoid loosing Serial port connexion)
                dataToSend = {};
                int delayMs = 1000 * 60 * 15;
#ifdef DEBUG
                delay(delayMs);
#else
                LowPower.deepSleep(delayMs);
#endif

                baroSensor.resetState();
                luxSensor.resetState();
                soilSensor.resetState();
                sfm.resetState();

                sending = false;
            }
        }
        else
        {
            // Ensure sensors switch is active
            sensorsSwitch.switchState(true);
            logger.e("One sensor data not ready");
        }

        startTimeMs = millis();
    }
}