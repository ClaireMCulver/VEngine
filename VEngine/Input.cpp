#include "Input.h"

Input* Input::Singleton = nullptr;

Input::Input()
{
	if (!Singleton)
	{
		Singleton = this;
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