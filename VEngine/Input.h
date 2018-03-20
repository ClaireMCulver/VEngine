#pragma once

#include "WinKeyboard.h"
#include "WinMouse.h";

class Input
{
public:
	Input();
	~Input();

	void UpdateInput();

public:
	WinKeyboard keyboard;
	WinMouse mouse;
	static Input* singleton;
};

