#include "Clock.h"

float Clock::fixedDeltaTime = 0.16666667f;

Clock* Clock::singleton = nullptr;

Clock::Clock()
{
	previousClock = std::clock();

	singleton = this;
}


Clock::~Clock()
{
}

void Clock::Tick()
{
	currentClock = std::clock();
	deltaTime = (currentClock - previousClock) / static_cast<double>(CLOCKS_PER_SEC);
	previousClock = currentClock;
}
