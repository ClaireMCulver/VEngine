#pragma once

#include <Windows.h>
#include <stdio.h>
#include <string>

#include <assert.h>

#include "Input.h"

struct WindowData
{
	HINSTANCE connection;
	HWND window;
};

class Win32Window
{
public:
	Win32Window(std::string WindowName, int xResolution, int yResolution, bool isFullscreen);
	~Win32Window();

	const WindowData GetWindowData() const { return windowData; }
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	WindowData windowData;
	
	Input* inputPtr;
};

