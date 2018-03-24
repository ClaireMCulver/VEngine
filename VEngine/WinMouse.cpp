#include "WinMouse.h"

WinMouse::WinMouse()
{
}

WinMouse::~WinMouse()
{
}

void WinMouse::OnMouseDown()
{
	
}

void WinMouse::OnMouseUp()
{
}

void WinMouse::OnMouseMove(HWND inhWnd)
{
	GetCursorPos(&mousePosition.point);
	ScreenToClient(inhWnd, &mousePosition.point);
}