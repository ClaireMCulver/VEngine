#pragma once

#include <Windows.h>
#include <stdio.h>
#include <string>

#include <assert.h>

struct WindowData
{
	HINSTANCE connection;
	HWND window;
};

class Win32Window
{
public:
	Win32Window();
	Win32Window(float xResolution, float yResolution, bool fullscreen = false);
	~Win32Window();

	void SetwindowData(int width, int height) { windowSize[0] = (float)width; windowSize[1] = (float)height; }
	const WindowData GetWindowData() const { return windowData; }

	void InitWindow(std::string WindowName, bool isFullscreen = false);

	Win32Window* GetSingleton() { return singleton; }

private:
	static Win32Window* singleton;
	WindowData windowData;

	float windowSize[2];
	bool isFullscreen = false;

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
