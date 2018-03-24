#pragma once

#include "WinKeyboard.h"
#include "WinMouse.h"

class Input
{
public:
	Input();
	~Input();

	void UpdateInput();

	inline WinKeyboard* GetKeyboard() { return &keyboard; }
	inline WinMouse* GetMouse() { return &mouse; }

public:
	static Input* singleton;

private:
	WinKeyboard keyboard;
	WinMouse mouse;
};
