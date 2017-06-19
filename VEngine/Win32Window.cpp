#include "Win32Window.h"

Win32Window* Win32Window::singleton = NULL;

Win32Window::Win32Window()
{
	assert(singleton == NULL);
	singleton = this;

	windowSize[0] = 800;
	windowSize[1] = 600;
	isFullscreen = false;
}

Win32Window::Win32Window(float xResolution, float yResolution, bool fullscreen)
{
	windowSize[0] = xResolution;
	windowSize[1] = yResolution; 
	isFullscreen = fullscreen;
}

Win32Window::~Win32Window()
{
	DestroyWindow(windowData.window);
}

void Win32Window::InitWindow(std::string WindowName, bool isFullscreen)
{
	windowData.connection = GetModuleHandle(NULL);

	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = windowData.connection;  // hInstance
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "Dragons";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass)) {
		// It didn't work, so try to give a useful error:
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}

	RECT windowRect = { 0, 0, (LONG)windowSize[0], (LONG)windowSize[1] };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	windowData.window = CreateWindowEx(0, WindowName.c_str(), WindowName.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU, 100, 100, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, windowData.connection, NULL);
	if (!windowData.window)
	{
		printf("failed to create a window in which to draw\n");
		fflush(stdout);
		exit(1);
	}
}

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	struct sample_info *info = reinterpret_cast<struct sample_info *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		//run(info); //Placeholder function for samples that want to show dynamic content
		return 0;
	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}