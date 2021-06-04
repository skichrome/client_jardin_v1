
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
        byte line[12] = {};
        int dataIdx = 0;

        while (char c = configFile.read())
        {
            if (c == EOF)
                break;

            if (c == '\n')
            {
                switch (dataIdx++)
                {
                case 1:
                    startTime = (int)line;
                    break;

                default:
                    break;
                }

                for (unsigned int i = 0; i < sizeof(line); i++)
                    line[i] = 0;
            }
        }

        // logger->e("Config Text: " + text);
    }
}

void SprinkleCommand::sprinkle()
{
}