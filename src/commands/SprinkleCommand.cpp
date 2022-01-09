
#include "commands/SprinkleCommand.h"

SprinkleCommand::SprinkleCommand(Logger *mLogger, RTCZero *mRtc) : logger(NULL), rtc(NULL)
{
    logger = mLogger;
    rtc = mRtc;
    state = SprinkleCommand::IDLE;
}

void SprinkleCommand::setup()
{
    configureSDCard();
    relay.setup();
}

void SprinkleCommand::loop()
{
    relay.loop();

    switch (state)
    {
    case SprinkleCommand::NO_CONF:
    {
        loadConfig();
        state = SprinkleCommand::IDLE;
        break;
    }
    case SprinkleCommand::IDLE:
    {
        break;
    }
    case SprinkleCommand::SPRINKLE_START:
    {
        relay.switchRelay();
        state = SprinkleCommand::SPRINKLE_RUN;
        break;
    }
    case SprinkleCommand::SPRINKLE_END:
    {
        relay.switchRelay();
        endSprinkle();
        state = SprinkleCommand::SPRINKLE_DONE;
        break;
    }

    default:
        break;
    }
}

void SprinkleCommand::configureSDCard()
{
    if (!SD.begin(4))
        logger->e(F("Can't configure SD Card lib in SprinkleCommand"));
}

void SprinkleCommand::loadConfig()
{
    if (!SD.exists(configFileName))
    {
        state = SprinkleCommand::NO_CONF;
        logger->e(F("No sprinkle configuration file found, sprinkle disabled"));
    }

    File configFile = SD.open(configFileName, FILE_READ);

    if (configFile)
    {
        String line = "";
        int lineIdx = 0;

        while (configFile.available())
        {
            uint16_t c = configFile.read();

            if (c == '\n')
            {
                //Serial.print(" Converted to line : ");
                //Serial.println(line);

                line = "";
                lineIdx++;

                continue;
            }

            if (c >= 48 && c <= 57)
            {
                //Serial.print(" Char : ");
                //Serial.print(c - 48);
                line += char(c);

                switch (lineIdx)
                {
                case 0:
                    // requestedTimestamp = line.toInt();
                    break;
                case 1:
                    startTimeHour = line.toInt();
                    break;
                case 2:
                    startTimeMin = line.toInt();
                    break;
                case 3:
                    duration = line.toInt();
                    break;
                case 4:
                    minimalSprinkleHumidity = line.toInt();
                    break;
                case 5:
                    lastHumidityValue = line.toInt();
                    break;

                default:
                    logger->e(F("Dropped line "));
                    logger->e(line);
                    break;
                }
            }
            else
                logger->e(F("Bad char value stored"));
        }
        configFile.close();
    }
    else
    {
        logger->e(F("Can't open config file, cancelling sprinkle"));
        return;
    }

    SD.end();
}

int SprinkleCommand::checkIfSprinkleIsRequired()
{
    if (state != SprinkleCommand::IDLE)
        return -2;

    if (lastHumidityValue > minimalSprinkleHumidity)
        return -3;

    int minutesDelta = rtc->getMinutes() - startTimeMin;
    unsigned int requestedDurationMillis = 1000 * 60 * duration;

    configureSDCard();

    File lastSprinkleFile = SD.open(lastSprinkleFileName, FILE_READ);
    String lastDay = "";

    if (lastSprinkleFile)
    {
        while (lastSprinkleFile.available())
        {
            uint16_t c = lastSprinkleFile.read();
            if (c == '\n')
                break;

            lastDay += char(c);
        }

        lastSprinkleFile.close();
    }
    else
        logger->e(F("Last sprinkle file not available"));

    if (lastDay == "")
        lastDay = "-1";

    if (lastDay.toInt() != rtc->getDay() && rtc->getHours() == startTimeHour && abs(minutesDelta) < duration * 2)
    {
        logger->e(F("Sprinkle not done today"));
        SD.end();
        return requestedDurationMillis;
    }
    else
    {
        logger->e(F("Sprinkle already done today or not time to sprinkle"));
        logger->e("Last sprinkle day: " + lastDay + " | current day: " + String(rtc->getDay()));
        logger->e("Requested sprinkle hour: " + String(startTimeHour) + " | current hour: " + String(rtc->getHours()));
        logger->e("Requested sprinkle min: " + String(startTimeMin) + " | current min: " + String(minutesDelta));
    }

    SD.end();
    return -1;
}

void SprinkleCommand::endSprinkle()
{
    configureSDCard();

    if (SD.exists(lastSprinkleFileName))
        SD.remove(lastSprinkleFileName);

    File lastSprinkleFile = SD.open(lastSprinkleFileName, FILE_WRITE);

    if (lastSprinkleFile)
    {
        lastSprinkleFile.print(rtc->getDay());
        lastSprinkleFile.close();
    }

    File sprinkleHistory = SD.open(sprinkleHistoryFileName, FILE_WRITE);
    if (sprinkleHistory)
    {
        sprinkleHistory.print(rtc->getDay());
        sprinkleHistory.close();
        logger->e(F("Wrote current day to sprinkle history"));
    }
    else
        logger->e(F("Could not open last sprinkle file"));

    SD.end();
}

void SprinkleCommand::requestStartSprinkle()
{
    state = SprinkleCommand::SPRINKLE_START;
}

void SprinkleCommand::requestEndSprinkle()
{
    state = SprinkleCommand::SPRINKLE_END;
}