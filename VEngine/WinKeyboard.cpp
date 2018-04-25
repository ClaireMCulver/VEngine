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
	USHORT value = GetKeyState(VkKeyScan(key)) & 128;
	return (value == 128);
}

bool WinKeyboard::IsKeyDown(int virtualKeyCode)
{
	USHORT value = GetKeyState(virtualKeyCode) & 128;
	return (value == 128);
}

bool WinKeyboard::IsKeyUp(char key)
{
	USHORT value = GetKeyState(VkKeyScan(key)) & 128;
	return (value != 128);
}

bool WinKeyboard::IsKeyUp(int virtualKeyCode)
{
	USHORT value = GetKeyState(virtualKeyCode) & 128;
	return (value != 128);
}