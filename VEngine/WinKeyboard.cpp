#include "WinKeyboard.h"



WinKeyboard::WinKeyboard()
{
}


WinKeyboard::~WinKeyboard()
{
}

void WinKeyboard::UpdateKeyboardState()
{
	GetKeyboardState(keyboardState);
}

bool WinKeyboard::IsKeyDown(char key)
{
	return (GetKeyState(key) == 128);
}