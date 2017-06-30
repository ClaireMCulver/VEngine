#pragma once

#include <exception>

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

#include "Graphics.h"
#include "Win32Window.h"
#include "GameDefs.h"

class SwapChain
{
public:
	SwapChain(GraphicsSystem &graphicsSystem, int xResolution, int yResolution, bool fullscreen);
	~SwapChain();

private:
	//Swapchain
	VkSwapchainKHR vkSwapchain; 
	uint32_t swapchainImageCount = 2; //Number of images in the swapchain.
	VkFormat SwapChainImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	uint32_t currentImage = 0;
	VkQueue presentationQueue;
	VkPresentInfoKHR presentInfo;

	CommandBuffer* blitBuffer;

	VkSemaphore imageAcquireSignal;
	VkSemaphore imageTransferedSignal;

	//Windows surface
	VkSurfaceKHR vkSurface;

	//OS Window handle and info
	Win32Window* win32Window;
	float windowSize[2];
	bool isFullscreen = false;

	//Handles to graphics system
	GraphicsSystem* pGraphicsSystem;

public:
	//Transfers the image passed to the swapchain as part of a command buffer.
	void BlitToSwapChain(VkCommandBuffer cmdBuffer, VkImage srcImage, VkImageLayout srcImageLayout);

	//Present the next image in the swapchain
	void PresentNextImage();

private:
	void InitializeSurface(GraphicsInstance* instance);

};