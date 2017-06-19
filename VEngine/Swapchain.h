#pragma once

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

class Swapchain
{
public:
	Swapchain();
	~Swapchain();

private:
	VkSwapchainKHR swapchain; 
	uint32_t swapchainImageCount = 2; //Number of images in the swapchain.
	std::vector<SwapchainBuffer> swapchainBuffers;

private:
	void initSwapchain();
	void destroySwapchain();


};

