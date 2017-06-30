#pragma once

//Cpp Core
#include <vector>

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

#include "GraphicsPhysicalDevice.h"

class GraphicsLogicalDevice
{
public:
	GraphicsLogicalDevice(GraphicsPhysicalDevice &physicalDevice);
	~GraphicsLogicalDevice();

private:
	VkDevice vkLogicalDevice;
	std::vector<const char*> deviceExtentions;
public:
	VkDevice GetVKLogicalDevice() const { return vkLogicalDevice; }

};

