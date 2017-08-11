#include "Clock.h"

Clock::Clock()
{
	previousClock = std::clock();
}


Clock::~Clock()
{
}

void Clock::Tick()
{
	currentClock = std::clock();
	deltaTime = (currentClock - previousClock) / 10;
	previousClock = currentClock;
}
