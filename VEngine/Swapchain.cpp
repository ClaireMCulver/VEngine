#include "Swapchain.h"



SwapChain::SwapChain(GraphicsSystem &graphicsSystem, int xResolution, int yResolution, bool fullscreen)
{
	VkResult result;

	const VkDevice vkLogicalDevice = graphicsSystem.GetLogicalDevice()->GetLogicalDevice();
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
	presentationSupported |= vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.graphicsQueueIndex);
	presentationSupported |= vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.computeQueueIndex);
	presentationSupported |= vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.transferQueueIndex);
	presentationSupported |= vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice, indices.sparseQueueIndex);
	assert(presentationSupported == VK_TRUE);


	// On to initialization //

	// Initialize the app window on the os //
	win32Window = new Win32Window(GameName, xResolution, yResolution, fullscreen);

	// Initialize the surface we are going to render to //
	InitializeSurface(graphicsSystem.GetInstance());

	// Create info for the swapchain //
	VkSwapchainCreateInfoKHR swapchainCI;
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = NULL;
	swapchainCI.flags = 0;
	swapchainCI.surface = vkSurface;
	swapchainCI.minImageCount = this->swapchainImageCount;
	swapchainCI.imageFormat = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
	swapchainCI.imageColorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCI.imageExtent = { (uint32_t)xResolution, (uint32_t)yResolution };
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapchainCI.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = familyIndices.size();
	swapchainCI.pQueueFamilyIndices = familyIndices.data();
	swapchainCI.preTransform = VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
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

}


SwapChain::~SwapChain()
{
	delete win32Window;

	//Destroy swapchain;
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
	assert(result);
}