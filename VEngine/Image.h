#pragma once

#include <assert.h>

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Graphics.h"

class Image
{
public:
	Image(int imageWidth, int imageHeight, VkFormat imageFormat, VkImageUsageFlags usage, VkImageAspectFlags imageAspect);
	~Image();

	VkImage GetImage() const { return vkImage; }
	VkImageView GetImageView() const { return vkImageView; }
	VkClearValue GetClearValue() const { return vkClearColour; }

private:
	VkImage vkImage;
	VkImageView vkImageView;
	VkDeviceMemory vkDeviceMemory;
	VkClearValue vkClearColour;

private:
	uint32_t FindMemoryType(uint32_t typeBits, VkFlags requirements_mask);
};

