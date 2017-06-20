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
	VkDevice logicalDevice;

public:
	VkDevice GetLogicalDevice() const { return logicalDevice; }

};

