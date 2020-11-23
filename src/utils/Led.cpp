/*
* Setup a led to be used for debug purposes: input a number (x) to blinkErrorCode(x) and
* this class will blink the led x times, and make a pause before re-blink.
*
* Note that if you re-call blinkErrorCode before the previous blink is ended the code will
* not be blinked.
*/

#include "Led.h"

void Led::setup()
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, Led::DISABLED);
}

void Led::loop()
{
	switch (cycleState)
	{
	case Led::WAITING:
		if (cycleCount > 0)
		{
			digitalWrite(pin, Led::DISABLED);
			startDurationMs = millis();
			cycleState = Led::POWER_ON;
		}
		break;

	case Led::POWER_ON:
		if (millis() - startDurationMs > HIGH_DURATION_MS)
		{
			digitalWrite(pin, Led::DISABLED);
			startDurationMs = millis();
			cycleState = Led::POWER_OFF;
			cycleCount--;
		}
		break;
	case Led::POWER_OFF:
		if (millis() - startDurationMs > LOW_DURATION_MS)
		{
			digitalWrite(pin, Led::ENABLED);
			startDurationMs = millis();
			cycleState = Led::POWER_ON;
		}
		break;
	case Led::PAUSING:
		if (millis() - startDurationMs > PAUSING_DURATION_MS)
		{
			startDurationMs = millis();
			cycleState = Led::WAITING;
		}
		break;
	default:
		cycleState = Led::WAITING;
		cycleCount = 0;
		startDurationMs = 0L;
		break;
	}
	if (cycleCount <= 0)
		cycleState = Led::PAUSING;
}

void Led::blinkErrorCode(unsigned int code)
{
	if (cycleCount != 0)
		return;

	cycleCount = code + 1;
}