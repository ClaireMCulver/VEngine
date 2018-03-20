#include "Win32Window.h"


Win32Window::Win32Window(std::string WindowName, int xResolution, int yResolution, bool isFullscreen)
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
	windowClass.lpszClassName = "v_WindowClass";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass)) {
		// It didn't work, so try to give a useful error:
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}

	RECT windowRect = { 0, 0, (LONG)xResolution, (LONG)yResolution };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	windowData.window = CreateWindowEx(0, windowClass.lpszClassName, WindowName.c_str(), WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPEDWINDOW, 100, 100, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, windowData.connection, NULL);

	//store this pointer.
	SetWindowLongPtr(windowData.window, GWLP_USERDATA, (LONG_PTR)this);

	winMousePtr = &Input::singleton->mouse;

	assert(windowData.window);
}

Win32Window::~Win32Window()
{
	DestroyWindow(windowData.window);
}

// MS-Windows event handling function:
LRESULT CALLBACK Win32Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	//Get 'this' pointer back.
	Win32Window* ThisPtr = reinterpret_cast<Win32Window*>((intptr_t)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_USERDATA));	
	
	switch (uMsg) 
	{

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		//run(info); //Placeholder function for samples that want to show dynamic content
		return 0;

	case WM_SETFOCUS:
		SetFocus(hWnd);
		break;

	case WM_LBUTTONDOWN:
		GetFocus();
		
	
		ThisPtr->winMousePtr->OnMouseDown(hWnd);


		break;

	case WM_LBUTTONUP:

		ThisPtr->winMousePtr->OnMouseUp();

	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}