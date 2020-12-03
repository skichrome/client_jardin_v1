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
            radio.stopListening();
            state = WirelessManager::SENDING;
        }
        break;
    case WirelessManager::SENDING:
        if (!radio.write(&rfMessage, sizeof(rfMessage)))
        {
            Serial.println("Sending values failed...");
        }
        radio.startListening();
        state = WirelessManager::WAITING_RESPONSE;
        timeoutMs = millis();

        break;
    case WirelessManager::WAITING_RESPONSE:

        if (millis() - timeoutMs > TIMEOUT_MS && !radio.available())
            state = WirelessManager::WAITING_DATA_TO_SEND;

        if (radio.available())
        {
            radio.read(&response, sizeof(response));
            Serial.print("Response: ID: ");
            Serial.println(response.id);
            Serial.print("Response: value: ");
            Serial.println(response.lux);
        }

        break;

    default:
        break;
    }
}

void WirelessManager::setLux(long mLux)
{
    rfMessage.lux = mLux;
}