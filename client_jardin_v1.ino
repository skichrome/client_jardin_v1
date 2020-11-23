/*
 Name:		client_jardin_v1.ino
 Created:	22/11/2020 20:34:42
 Author:	Toni
*/

#include "src/utils/Runnable.h"
#include <src/utils/Led.h>

Runnable* Runnable::headRunnable = NULL;

Led debugLed = Led(LED_BUILTIN);

unsigned long delayMs = 0L;

void setup()
{
	Runnable::setupAll();
}

void loop()
{
	Runnable::loopAll();

	if (millis() - delayMs > 10000L)
	{
		debugLed.blinkErrorCode(2);
		delayMs = millis();
	}
}
