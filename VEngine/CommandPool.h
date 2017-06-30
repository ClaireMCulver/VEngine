#pragma once

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#include "GraphicsLogicalDevice.h"

class CommandPool
{
public:
	CommandPool(GraphicsLogicalDevice &logicalDevice, uint32_t commandBufferQueueFamily);
	~CommandPool();

private:
	VkCommandPool vkCommandPool;

	VkDevice vkLogicalDevice;

public:
	VkCommandPool GetVKCommandPool() const { return vkCommandPool; }
	VkDevice GetVkLogicalDevice() const { return vkLogicalDevice; }
};

