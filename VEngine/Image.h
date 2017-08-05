#pragma once

#include <assert.h>

#include "glm\vec2.hpp"

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Graphics.h"
#include "CommandBuffer.h"


class Image
{
public:
	Image(int imageWidth, int imageHeight, VkFormat imageFormat, VkImageUsageFlags usage, VkImageAspectFlags imageAspect);
	~Image();

	//Immediately change the layout of the image.
	void ChangeImageLayout(VkImageLayout newLayout);

	//Change the layout of the image as part of a larger command.
	void ChangeImageLayout(CommandBuffer& commandBuffer, VkImageLayout newLayout);

	VkImage GetImage() const { return vkImage; }
	VkImageView GetImageView() const { return vkImageView; }
	VkClearValue GetClearValue() const { return vkClearColour; }
	VkImageLayout GetImageLayout() const { return vkImageLayout; }
	glm::vec2 GetImageSize() const { return imageSize; }

	void CmdClearImage(CommandBuffer &commandBuffer);

protected:
	//Vulkan Handles
	VkImage vkImage;
	VkImageView vkImageView;
	VkDeviceMemory vkDeviceMemory;
	VkClearValue vkClearColour;

	//Image properties
	glm::vec2 imageSize;
	VkImageLayout vkImageLayout;
	VkImageAspectFlags vkImageAspect;

private:
	uint32_t FindMemoryType(uint32_t typeBits, VkFlags requirements_mask);
};

