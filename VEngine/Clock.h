#pragma once

#include <ctime>

class Clock
{
public:
	Clock();
	~Clock();

	void Tick();

private:
	double currentClock;
	double previousClock;
	double deltaTime;
};

