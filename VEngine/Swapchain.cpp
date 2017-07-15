#include "Swapchain.h"



SwapChain::SwapChain(GraphicsSystem &graphicsSystem, int xResolution, int yResolution, bool fullscreen)
{
	VkResult result;

	pGraphicsSystem = &graphicsSystem;
	const VkDevice vkLogicalDevice = graphicsSystem.GetLogicalDevice()->GetVKLogicalDevice();
	const VkPhysicalDevice vkPhysicalDevice = graphicsSystem.GetPhysicalDevice()->GetPhysicalDevice();

	// Get the queue family indices for the physical device //
	const GraphicsPhysicalDevice::QueueFamilyIndices indices = graphicsSystem.GetPhysicalDevice()->GetQueueFamilyIndices();

	//Push back the queue family indices that we are actually going to end up using.
	std::vector<uint32_t> familyIndices;
	familyIndices.push_back(indices.graphicsQueueIndex);
	familyIndices.push_back(indices.transferQueueIndex);
	//probably add compute and sparce later, but this is fine for now.

	// Check for if the specified queue family supports presentation //
	VkBool32 presentationSupported = false;
	if (vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.graphicsQueueIndex) == VK_TRUE)
	{
		presentationSupported |= true;
		presentationQueue = graphicsSystem.GetGraphicsQueue();
	}
	if (vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.computeQueueIndex) == VK_TRUE)
	{
		presentationSupported |= true;
		presentationQueue = graphicsSystem.GetTransferQueue();
	}
	if (vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.transferQueueIndex) == VK_TRUE)
	{
		presentationSupported |= true;
		presentationQueue = graphicsSystem.GetTransferQueue();
	}
	if (vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.sparseQueueIndex) == VK_TRUE)
	{
		presentationSupported |= true;
		presentationQueue = graphicsSystem.GetSparseQueue();
	}
	assert(presentationSupported == VK_TRUE);

	// On to initialization //

	// Initialize the app window on the os //
	win32Window = new Win32Window(GameName, xResolution, yResolution, fullscreen);

	// Initialize the surface we are going to render to //
	InitializeSurface(graphicsSystem.GetInstance());

	// Surface Validation //
	//Check that the surface is supported by at least one queue
	VkBool32 graphicsSurfaceSupported = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, indices.graphicsQueueIndex, vkSurface, &graphicsSurfaceSupported);

	VkBool32 computeSurfaceSupported = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, indices.graphicsQueueIndex, vkSurface, &computeSurfaceSupported);	
	
	VkBool32 transferSurfaceSupported = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, indices.graphicsQueueIndex, vkSurface, &transferSurfaceSupported);	
	
	VkBool32 sparseSurfaceSupported = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, indices.graphicsQueueIndex, vkSurface, &sparseSurfaceSupported);

	if (!(graphicsSurfaceSupported || computeSurfaceSupported || transferSurfaceSupported || sparseSurfaceSupported))
	{
		printf("Surface not supported");

		exit(1);
	}

	//Fetch the surface formats
	uint32_t surfaceFormatCount;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, NULL);
	surfaceFormats.resize(surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, vkSurface, &surfaceFormatCount, surfaceFormats.data());



	// Create info for the swapchain //
	VkSwapchainCreateInfoKHR swapchainCI;
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = NULL;
	swapchainCI.flags = 0;
	swapchainCI.surface = vkSurface;
	swapchainCI.minImageCount = this->swapchainImageCount;
	swapchainCI.imageFormat = SwapChainImageFormat;
	swapchainCI.imageColorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCI.imageExtent = { (uint32_t)xResolution, (uint32_t)yResolution };
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapchainCI.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = familyIndices.size();
	swapchainCI.pQueueFamilyIndices = familyIndices.data();
	swapchainCI.preTransform = VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCI.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCI.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.oldSwapchain = VK_NULL_HANDLE;

	// Get the presentation properties of the device //
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, vkSurface, &surfaceCapabilities);

	// Do validation on the swapchain creation //
	//nah

	result = vkCreateSwapchainKHR(vkLogicalDevice, &swapchainCI, NULL, &vkSwapchain);
	assert(result == VK_SUCCESS);


	//Fetch the swapchain's images for creation and manipulation.
	vkGetSwapchainImagesKHR(vkLogicalDevice, vkSwapchain, &swapchainImageCount, nullptr);
	swapchainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(vkLogicalDevice, vkSwapchain, &swapchainImageCount, swapchainImages.data());

	swapchainImageViews.resize(swapchainImageCount);
	for (int i = 0, count = swapchainImageViews.size(); i < count; i++)
	{
		VkImageViewCreateInfo imageViewCI;
		imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCI.pNext = NULL;
		imageViewCI.flags = 0;
		imageViewCI.image = swapchainImages[i];
		imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCI.format = swapchainCI.imageFormat;
		imageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
		imageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
		imageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
		imageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
		imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCI.subresourceRange.baseMipLevel = 0;
		imageViewCI.subresourceRange.levelCount = 1;
		imageViewCI.subresourceRange.baseArrayLayer = 0;
		imageViewCI.subresourceRange.layerCount = 1;

		result = vkCreateImageView(vkLogicalDevice, &imageViewCI, NULL, &swapchainImageViews[i]);
		assert(result == VK_SUCCESS);
	}
	
	// Semaphore Creation //
	VkSemaphoreCreateInfo imageAcquiredSignalInfo;
	imageAcquiredSignalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageAcquiredSignalInfo.pNext = NULL;
	imageAcquiredSignalInfo.flags = 0;
	vkCreateSemaphore(vkLogicalDevice, &imageAcquiredSignalInfo, NULL, &imageAcquireSignal);

	VkSemaphoreCreateInfo imageTransferedSignalInfo;
	imageTransferedSignalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageTransferedSignalInfo.pNext = NULL;
	imageTransferedSignalInfo.flags = 0;
	vkCreateSemaphore(vkLogicalDevice, &imageTransferedSignalInfo, NULL, &imageTransferedSignal);

	// Presentation Information //
	currentImage = 0;
	//The semaphores might change, but so I'll put all this here for a simple optimization.
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &imageTransferedSignal;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vkSwapchain;
	presentInfo.pImageIndices = &currentImage;
	presentInfo.pResults = NULL;

	// Blit buffer creation //
	blitBuffer = new CommandBuffer(CommandBufferType::Transfer, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	blitBuffer->AddWaitSemaphore(imageAcquireSignal);
	blitBuffer->AddSignalSemaphore(imageTransferedSignal);
}


SwapChain::~SwapChain()
{
	delete win32Window;

	const VkDevice vkLogicalDevice = pGraphicsSystem->GetLogicalDevice()->GetVKLogicalDevice();

	vkDeviceWaitIdle(vkLogicalDevice);

	//Destroy swapchain images?

	for (size_t i = 0, count = swapchainImageViews.size(); i < count; i++)
	{
		vkDestroyImageView(vkLogicalDevice, swapchainImageViews[i], NULL);
	}

	vkDestroySwapchainKHR(vkLogicalDevice, vkSwapchain, NULL); //Note: Swapchains must be destroyed before the surface which depends on them.

	vkDestroySurfaceKHR(pGraphicsSystem->GetInstance()->GetInstance(), vkSurface, NULL);

	vkDestroySemaphore(vkLogicalDevice, imageAcquireSignal, NULL);
	vkDestroySemaphore(vkLogicalDevice, imageTransferedSignal, NULL);
}


void SwapChain::InitializeSurface(GraphicsInstance* instance)
{
	VkResult result;

	WindowData windowData = win32Window->GetWindowData();

	VkWin32SurfaceCreateInfoKHR surfaceCI;
	surfaceCI.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCI.pNext = NULL;
	surfaceCI.flags = 0;
	surfaceCI.hinstance = windowData.connection;
	surfaceCI.hwnd = windowData.window;

	result = vkCreateWin32SurfaceKHR(instance->GetInstance(), &surfaceCI, NULL, &vkSurface);
	assert(result == VK_SUCCESS);
}

void SwapChain::PresentNextImage()
{
	//This should actually be moved to where I blit the rendered buffer to the current image.


	//Decided to have presentation within the swapchain rather than in a job system, since it's rather independant, given it's dependence on everything else being done. 
	//If that makes any sense.
	vkQueuePresentKHR(presentationQueue, &presentInfo);
}

void SwapChain::BlitToSwapChain(VkCommandBuffer cmdBuffer, VkImage srcImage, VkImageLayout srcImageLayout)
{
	const VkDevice vkLogicalDevice = pGraphicsSystem->GetLogicalDevice()->GetVKLogicalDevice();
	vkAcquireNextImageKHR(vkLogicalDevice, vkSwapchain, UINT64_MAX, imageAcquireSignal, NULL, &currentImage);

	VkImageBlit blitRegion;
	blitRegion.srcSubresource = 
	{
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, //aspectMask;
		0, //mipLevel;
		0, //baseArrayLayer;
		1  //layerCount;
	};
	blitRegion.srcOffsets[0] = { 0, 0, 0 };
	//TODO: Image class? Framebuffer class?
	blitRegion.srcOffsets[1] = { 0, 0, 1 };
	blitRegion.dstSubresource = 
	{
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, //aspectMask;
		0, //mipLevel;
		0, //baseArrayLayer;
		1  //layerCount;
	};
	blitRegion.dstOffsets[0] = { 0, 0, 0 };
	blitRegion.dstOffsets[1] = { (int32_t)windowSize[0], (int32_t)windowSize[1], 1 };
	
	// Record command buffer and submit it //
	blitBuffer->BeginRecording();

	vkCmdBlitImage(blitBuffer->GetVKCommandBuffer(), srcImage, srcImageLayout, swapchainImages[currentImage], VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, &blitRegion, VkFilter::VK_FILTER_NEAREST);

	blitBuffer->EndRecording();

	JobSystem::SubmitTransferJob(*blitBuffer);

	//Restart command buffer 
	blitBuffer->ResetBuffer();
}