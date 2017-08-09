#pragma once

#include "WinKeyboard.h"

class Input
{
public:
	Input();
	~Input();

	void UpdateInput();

public:
	WinKeyboard keyboard;
};

