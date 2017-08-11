#pragma once

#include <Windows.h>

#include <stdio.h>

class WinKeyboard
{
public:
	WinKeyboard();
	~WinKeyboard();
	
	//Sets the keyboard state to the current frame's state.
	void UpdateKeyboardState();

	//Returns the key's state on that frame.
	bool IsKeyDown(char key);

	bool IsKeyUp(char key);

private:
	BYTE keyboardState[256]; //Windows' GetKeyoardState function returns a 256 element array of one byte characters.
};

