#ifndef _WIRELESS_TRANSMITTER_H
#define _WIRELESS_TRANSMITTER_H

#include "utils/Runnable.h"

#include <RF24.h>

class WirelessManager : public Runnable
{
private:
    enum State
    {
        NOT_FOUND,
        WAITING_DATA_TO_SEND,
        SENDING,
        WAITING_RESPONSE
    } state = NOT_FOUND;

    struct dataStruct
    {
        unsigned long id;
        long lux = -1;
    } rfMessage;

    dataStruct response;

    byte addresses[2][6] = {"trsmt", "reciv"};
    RF24 radio;

    unsigned long timeoutMs = 0L;
    const unsigned long TIMEOUT_MS = 5000L;

protected:
    virtual void setup();
    virtual void loop();

public:
    WirelessManager(byte mCeAttach, byte mCsnAttach) : radio(RF24(mCeAttach, mCsnAttach))
    {
    }

    void setLux(long mLux);
};

#endif