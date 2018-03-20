#include "Input.h"

Input* Input::singleton = nullptr;

Input::Input()
{
	if (!singleton)
	{
		singleton = this;
	}
}


Input::~Input()
{
}

void Input::UpdateInput()
{
	MSG msg;
	GetMessage(&msg, nullptr, 0, 0);
	TranslateMessage(&msg);
	DispatchMessage(&msg);

	keyboard.UpdateKeyboardState();
}