#include "transmitter/WirelessManager.h"

void WirelessManager::setup()
{
    while (!radio.begin())
    {
        state = WirelessManager::NOT_FOUND;
    }

    radio.setPALevel(RF24_PA_LOW);
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);

    state = WirelessManager::WAITING_DATA_TO_SEND;
}

void WirelessManager::loop()
{
    switch (state)
    {
    case WirelessManager::WAITING_DATA_TO_SEND:
        if (rfMessage.lux != -1L)
        {
            state = WirelessManager::SENDING;
        }
        break;
    case WirelessManager::SENDING:
        if (!radio.write(&rfMessage, sizeof(rfMessage)))
        {
            Serial.println("Sending values failed...");
        }
        
        break;
    case WirelessManager::WAITING_RESPONSE:
        break;

    default:
        break;
    }
}

void WirelessManager::setLux(long mLux)
{
    rfMessage.lux = mLux;
}