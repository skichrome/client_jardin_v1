#include "utils/DebugLed.h"

DebugLed::DebugLed(byte attachTo)
{
	pin = attachTo;
}

void DebugLed::setup()
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, DebugLed::DISABLED);
}

void DebugLed::loop()
{
	switch (cycleState)
	{
	case DebugLed::WAITING:
		if (cycleCount > 0)
		{
			digitalWrite(pin, DebugLed::DISABLED);
			startDurationMs = millis();
			cycleState = DebugLed::POWER_ON;
		}
		break;

	case DebugLed::POWER_ON:
		if (millis() - startDurationMs > HIGH_DURATION_MS)
		{
			digitalWrite(pin, DebugLed::DISABLED);
			startDurationMs = millis();
			cycleState = DebugLed::POWER_OFF;
			cycleCount--;
		}
		break;
	case DebugLed::POWER_OFF:
		if (millis() - startDurationMs > LOW_DURATION_MS)
		{
			digitalWrite(pin, DebugLed::ENABLED);
			startDurationMs = millis();
			cycleState = DebugLed::POWER_ON;
		}
		break;
	case DebugLed::PAUSING:
		if (millis() - startDurationMs > PAUSING_DURATION_MS)
		{
			startDurationMs = millis();
			cycleState = DebugLed::WAITING;
		}
		break;
	default:
		cycleState = DebugLed::WAITING;
		cycleCount = 0;
		startDurationMs = 0L;
		break;
	}
	if (cycleCount <= 0)
		cycleState = DebugLed::PAUSING;
}

void DebugLed::blinkErrorCode(unsigned int code)
{
	if (cycleCount != 0)
		return;

	cycleCount = code + 1;
}