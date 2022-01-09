#include <Arduino.h>

#include <RTCZero.h>
#include <ArduinoLowPower.h>

#include "utils/Logger.h"
#include "utils/DebugLed.h"

// #include "sensors/LuxSensor.h"
#include "sensors/BaroSensor.h"
#include "sensors/SoilSensor.h"
#include "commands/SwitchCommand.h"
#include "commands/RelayCommand.h"
#include "remote/SigfoxManager.h"
#include "model/SensorsData.h"

#define SENSORS_COMMAND_SW 3
#define MOISTURE_SENSOR_PIN A0

RTCZero rtc;

DebugLed led = DebugLed(LED_BUILTIN);
Logger logger = Logger(&rtc, &led);

SensorsData dataToSend = SensorsData();

SigfoxManager sfm = SigfoxManager(&logger, &rtc);

// LuxSensor luxSensor = LuxSensor(&logger);
BaroSensor baroSensor = BaroSensor(&logger);
SoilSensor soilSensor = SoilSensor(&logger, MOISTURE_SENSOR_PIN);

SwitchCommand sensorsSwitch = SwitchCommand(&logger, SENSORS_COMMAND_SW);

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

    delay(1000);

    logger.e(F("Main setup done"));
}

void loop()
{
    led.loop();
    logger.loop();

    // To execute next part of code sensor sw must be on (case all sensors data OK but before sensors switch turn off)
    if (!sending && sensorsSwitch.isSwitchedOff())
    {
        logger.e(F("Bad sensor switch state, waiting next loop execution"));
        sensorsSwitch.switchState(true);
        return;
    }

    if (millis() - startTimeMs > 1000L)
    {
        logger.e("Sensor switch status is " + String(sensorsSwitch.isSwitchedOff()));
        if (/*luxSensor.isDataReady() && */baroSensor.isDataReady() && soilSensor.isDataReady())
        {
            // Sensors are connected and have collected data, get values
            // luxSensor.updateSensorData(&dataToSend);
            baroSensor.updateSensorsData(&dataToSend);
            soilSensor.updateSensorData(&dataToSend);

            // Todo : Set timestamp will trigger SigFox upload
            dataToSend.currentTimestamp = rtc.getEpoch();

            // Measures are done, reset sensor switch
            sensorsSwitch.switchState(false);

            sfm.sendData(&dataToSend);
            sending = true;

            if (sfm.isDataSent() && sensorsSwitch.isSwitchedOff())
            {
                // Reset and sleep (using delay if debug, to avoid loosing Serial port connexion)
                dataToSend = {};
                unsigned int delayMs = 1000 * 60 * 15;
                unsigned long delayTime = millis();
#ifdef DEBUG
                while (millis() - delayTime < delayMs)
                {
                    delay(30000);
                    logger.e(F("Sleeping in progress"));
                }
#else
                LowPower.deepSleep(delayMs);
#endif
                logger.e(F("wake up"));

                baroSensor.resetState();
                // luxSensor.resetState();
                soilSensor.resetState();
                sfm.resetState();

                sending = false;
            }
        }
        else
        {
            // Ensure sensors switch is active
            sensorsSwitch.switchState(true);
            logger.e(F("One sensor data not ready"));
        }

        startTimeMs = millis();
    }
}