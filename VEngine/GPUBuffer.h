#pragma once

#include <assert.h>
#include <vector>

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Graphics.h"

class GPUBuffer
{
public:
	GPUBuffer(VkBufferUsageFlags bufferUsage, void* data, uint64_t memorySize);
	~GPUBuffer();

private:
	VkBuffer vkBuffer;
	VkDeviceMemory vkMemory;

public:
	//public functions

private:

	uint32_t FindMemoryType(uint32_t typeBits, VkFlags requirements_mask);
};

