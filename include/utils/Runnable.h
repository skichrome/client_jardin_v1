#ifndef _RUNNABLE_H
#define _RUNNABLE_H

#include <Arduino.h>

class Runnable
{
private:
    static Runnable* headRunnable;
    Runnable* nextRunnable;

protected:
    virtual void setup() = 0;
    virtual void loop() = 0;

public:
    Runnable()
    {
        nextRunnable = headRunnable;
        headRunnable = this;
    }
    static void setupAll();
    static void loopAll();
};

#endif