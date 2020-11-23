#include "Runnable.h"

void Runnable::setupAll()
{
	for (Runnable* r = headRunnable; r; r = r->nextRunnable)
		r->setup();
}

void Runnable::loopAll()
{
	for (Runnable* r = headRunnable; r; r = r->nextRunnable)
		r->loop();
}
