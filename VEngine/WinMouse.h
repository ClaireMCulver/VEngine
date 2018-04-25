#pragma once

#include <Windows.h>
#include <assert.h>

#include "GameDefs.h"

union MousePosition
{
	POINT point;
	long xy[2];
};

class WinMouse
{
	friend class Win32Window;

public:
	WinMouse();
	~WinMouse();

private: //Friend functions

	// Gets the mouse input and converts it to window space for use.
	void OnMouseDown();

	void OnMouseUp();

	void OnMouseMove(HWND inhWnd);

public: //Get/Set

	inline MousePosition* GetMousePosition() { return &mousePosition; }
	inline bool GetMouseDown() { return isDown; }

private:
	MousePosition mousePosition;

	bool captured = false;
	bool isDown = false;

};