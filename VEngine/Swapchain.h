#pragma once

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

#include "Graphics.h"
#include "Win32Window.h"

class SwapChain
{
public:
	SwapChain(GraphicsSystem &graphicsSystem, int xResolution, int yResolution, bool fullscreen);
	~SwapChain();

private:
	//Swapchain
	VkSwapchainKHR vkSwapchain; 
	uint32_t swapchainImageCount = 2; //Number of images in the swapchain.
	std::vector<SwapchainBuffer> swapchainBuffers;

	//Windows surface
	VkSurfaceKHR vkSurface;

	//OS Window handle and info
	Win32Window* win32Window;
	float windowSize[2];
	bool isFullscreen = false;

private:

	void InitializeSurface(GraphicsInstance* instance);
};

