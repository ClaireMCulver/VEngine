#pragma once

//cpp core
#include <vector>

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

//Engine
#include "GraphicsSystem.h"

class FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height, VkFormat frameBufferFormat, bool useDepth, VkRenderPass renderPass);
	~FrameBuffer();

public:
	VkFramebuffer frameBuffer;
	std::vector<VkImage> images;
	std::vector<VkImageView> attachements;

};

