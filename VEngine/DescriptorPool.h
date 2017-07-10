#pragma once

#include <assert.h>
#include <vector>

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Graphics.h"

class DescriptorPool
{
public:
	DescriptorPool();
	~DescriptorPool();

private:
	VkDescriptorPool vkDescriptorPool;

public:
	VkDescriptorPool GetVKDescriptorPool() const { return vkDescriptorPool; }
};

