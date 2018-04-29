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
	VkImageAspectFlags GetImageAspect() const { return vkImageAspect; }
	glm::vec2 GetImageSize() const { return imageSize; }

	void CmdClearImage(CommandBuffer &commandBuffer);

public: //Static transfer functions

	//Blit image from source to destination as part of a command buffer
	static void CmdBlitImage(CommandBuffer &commandBuffer, Image* sourceImage, Image* destinationImage, VkFilter filter);

	struct ImageAccessData
	{
		VkFlags srcMask = 0;
		VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkFlags dstMask = 0;
		VkPipelineStageFlags dstStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	};
	static Image::ImageAccessData FetchImageAccessAndStage(VkImageLayout currentLayout, VkImageLayout newLayout);



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

	ImageAccessData FetchImageAccessAndStage(VkImageLayout newLayout);
};

