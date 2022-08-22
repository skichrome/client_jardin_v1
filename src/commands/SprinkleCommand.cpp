#include "config.h"
#include "commands/SprinkleCommand.h"

SprinkleCommand::SprinkleCommand(Logger *mLogger, RTCZero *mRtc) : logger(NULL), rtc(NULL)
{
    logger = mLogger;
    rtc = mRtc;
    state = SprinkleCommand::NO_CONF;
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
        logger->e(F("[SprinkleConfig] Can't configure SD Card lib in SprinkleCommand"));
}

void SprinkleCommand::loadConfig()
{
    if (!SD.exists(configFileName))
    {
        state = SprinkleCommand::NO_CONF;
        logger->e(F("[SprinkleConfig] No sprinkle configuration file found, sprinkle disabled"));
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
                line = "";
                lineIdx++;

                continue;
            }

            if (c >= 48 && c <= 57)
            {
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
                    logger->e(F("[SprinkleConfig] Dropped line "));
                    logger->e(line);
                    break;
                }
            }
            else
                logger->e("[SprinkleConfig] Out of range char value : " + String(c));
        }
        configFile.close();
    }
    else
    {
        logger->e(F("[SprinkleConfig] Can't open config file, cancelling sprinkle"));
        return;
    }
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
        logger->e(F("[SprinkleConfig] Last sprinkle file not available"));

    if (lastDay == "")
        lastDay = "-1";

    if (lastDay.toInt() != rtc->getDay() && rtc->getHours() == startTimeHour && abs(minutesDelta) < SLEEP_DELAY * 2)
    {
        logger->e(F("[SprinkleConfig] Sprinkle not done today"));
        return requestedDurationMillis;
    }
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
        sprinkleHistory.print("/");
        sprinkleHistory.print(rtc->getMonth());
        sprinkleHistory.print("/");
        sprinkleHistory.print(rtc->getYear());
        sprinkleHistory.print("[");
        sprinkleHistory.print(rtc->getHours());
        sprinkleHistory.print(":");
        sprinkleHistory.print(rtc->getMinutes());
        sprinkleHistory.println("]");
        sprinkleHistory.close();
        logger->e(F("[SprinkleConfig] Wrote current day to sprinkle history"));
    }
    else
        logger->e(F("[SprinkleConfig] Could not open sprinkle history file"));
}

void SprinkleCommand::requestStartSprinkle()
{
    state = SprinkleCommand::SPRINKLE_START;
}

void SprinkleCommand::requestEndSprinkle()
{
    state = SprinkleCommand::SPRINKLE_END;
}

bool SprinkleCommand::isSprinkleRunning()
{
    return state == SprinkleCommand::SPRINKLE_RUN && relay.isOn();
}

bool SprinkleCommand::isSprinkleOff()
{
    return (state == SprinkleCommand::SPRINKLE_DONE || state == SprinkleCommand::IDLE || state == SprinkleCommand::NO_CONF) && relay.isOff();
}

void SprinkleCommand::resetSprinkleState()
{
    if (state == State::SPRINKLE_DONE)
        state = State::NO_CONF;
}