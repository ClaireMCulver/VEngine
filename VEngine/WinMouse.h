#pragma once

#include <Windows.h>


class WinMouse
{
	friend class Win32Window;

public:
	WinMouse();
	~WinMouse();


private:
	static WinMouse* singleton;

private: //Friend functions

	// Gets the mouse input and converts it to window space for use.
	void OnMouseDown(HWND inhWnd);

	void OnMouseUp();

private:
	POINT point;
	int x, y;

	bool captured = false;

};