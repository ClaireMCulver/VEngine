//#include "vld.h"

#include <iostream>
#include <cstdlib>
#include <assert.h>

#include <vector>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "GraphicsInstance.h"
#include "GraphicsPhysicalDevice.h"
#include "GraphicsLogicalDevice.h"

void main()
{
	GraphicsInstance instance;
	GraphicsPhysicalDevice physicalDevices = GraphicsPhysicalDevice(instance);
	GraphicsLogicalDevice logicalDevice = GraphicsLogicalDevice(physicalDevices);

	return;
}