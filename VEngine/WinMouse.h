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

	void OnMouseDown(HWND inhWnd);
	void OnMouseUp();

private:
	LPPOINT lpPoint;
	int x, y;

};