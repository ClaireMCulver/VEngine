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
	GPUBuffer(VkBufferUsageFlags bufferUsage, uint64_t bufferSize);
	GPUBuffer(VkBufferUsageFlags bufferUsage, void* data, uint64_t bufferSize);
	~GPUBuffer();

	//Copies data into the buffer
	void CopyMemoryIntoBuffer(void* data, uint64_t memorySize);

private:
	VkBuffer vkBuffer;
	VkDeviceMemory vkMemory;
	uint64_t bufferSize;

public:
	//public functions
	VkBuffer GetVKBuffer() const			{ return vkBuffer; }
	VkDeviceMemory GetBufferMemory() const	{ return vkMemory; }

private:
	uint32_t FindMemoryType(uint32_t typeBits, VkFlags requirements_mask);

};

