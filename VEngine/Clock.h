#pragma once

#include <ctime>

class Clock
{
public:
	Clock();
	~Clock();

	void Tick();

	double DeltaTime() { return deltaTime; }

	static float fixedDeltaTime;

	static Clock* singleton;

private:
	double currentClock;
	double previousClock;
	double deltaTime;
};