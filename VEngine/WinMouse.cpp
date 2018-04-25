#include "WinMouse.h"

WinMouse::WinMouse()
{
}

WinMouse::~WinMouse()
{
}

void WinMouse::OnMouseDown()
{
	isDown = true;
}

void WinMouse::OnMouseUp()
{
	isDown = false;
}

void WinMouse::OnMouseMove(HWND inhWnd)
{
	GetCursorPos(&mousePosition.point);
	ScreenToClient(inhWnd, &mousePosition.point);
}