#include "WinMouse.h"

WinMouse* WinMouse::singleton = nullptr;

WinMouse::WinMouse()
{
	singleton = this;
}

WinMouse::~WinMouse()
{
}

void WinMouse::OnMouseDown(HWND inhWnd)
{
	GetCursorPos(lpPoint);


}

void WinMouse::OnMouseUp()
{
}
