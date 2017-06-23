#include "Swapchain.h"

#include "GameDefs.h"


SwapChain::SwapChain(GraphicsSystem &graphicsSystem, int xResolution, int yResolution, bool fullscreen)
{
	VkResult result;

	win32Window = new Win32Window(GameName, xResolution, yResolution, fullscreen);

	const VkDevice vkLogicalDevice = graphicsSystem.GetLogicalDevice()->GetLogicalDevice();

	InitializeSurface(graphicsSystem.GetInstance());

	const GraphicsPhysicalDevice::QueueFamilyIndices indices = graphicsSystem.GetPhysicalDevice()->GetQueueFamilyIndices();
	std::vector<uint32_t> familyIndices;
	familyIndices.push_back(indices.graphicsQueueIndex);
	familyIndices.push_back(indices.transferQueueIndex);

	VkSwapchainCreateInfoKHR swapchainCI;
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = NULL;
	swapchainCI.flags = 0;
	swapchainCI.surface = vkSurface;
	swapchainCI.minImageCount = this->swapchainImageCount;
	swapchainCI.imageFormat = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
	swapchainCI.imageColorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCI.imageExtent = { xResolution, yResolution };
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

	result = vkCreateSwapchainKHR(vkLogicalDevice, &swapchainCI, NULL, &vkSwapchain);
	assert(result == VK_SUCCESS);
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