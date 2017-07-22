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
	static DescriptorPool* singleton;

	VkDescriptorPool vkDescriptorPool;

public:
	static DescriptorPool* GetSingleton() { return singleton; }
	
	VkDescriptorPool GetVKDescriptorPool() const { return vkDescriptorPool; }
};

