#include "TimerSystem.h"


TimerSystem::TimerSystem()
{
	isActive = false;
	tick = tickStart = 0.0f;
}

TimerSystem::TimerSystem(float t)
{
	isActive = true;
	tick = tickStart = t;
};

void TimerSystem::Tick(float dt)
{
	if (!isActive) return;
	tick -= dt;
	if (tick <= 0) tick = 0;
}

bool TimerSystem::TimerEnd()
{
	return tick == 0;
}

void TimerSystem::TimerReset()
{
	tick = tickStart;
}
void TimerSystem::TimerReset(float newStart)
{
	tick = tickStart = newStart;
}