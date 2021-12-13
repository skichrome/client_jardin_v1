
#include "commands/SprinkleCommand.h"

SprinkleCommand::SprinkleCommand(Logger *mLogger) : logger(NULL)
{
    logger = mLogger;
    state = SprinkleCommand::IDLE;
}

void SprinkleCommand::setup()
{
    configureSDCard();
}

void SprinkleCommand::loop()
{
    switch (state)
    {
    case SprinkleCommand::NO_CONF:
        loadConfig();
        break;
    case SprinkleCommand::IDLE:
        break;

    default:
        break;
    }
}

void SprinkleCommand::configureSDCard()
{
    if (!SD.begin(4))
        logger->e(F("Can't configure SD Card lib in SprinkleCommand"));
    else
        logger->e(F("Successfully configured SprinkleCommand"));
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

                default:
                    //Serial.print("Dropped line ");
                    //Serial.println(line);
                    break;
                }
            }
            // else
            //Serial.print("Bad char value stored");
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

void SprinkleCommand::sprinkle()
{
}