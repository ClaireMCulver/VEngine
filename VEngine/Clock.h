#pragma once

#include <ctime>

class Clock
{
public:
	Clock();
	~Clock();

	void Tick();

	static float fixedDeltaTime;

private:
	double currentClock;
	double previousClock;
	double deltaTime;
};