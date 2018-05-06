#include "FrameBuffer.h"



FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, VkFormat frameBufferFormat, bool useDepth, VkRenderPass renderPass)
{
	images.reserve(1 + useDepth);
	attachements.reserve(1 + useDepth);

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	// Colour attachement creation //
	
	//image creation
	VkImageCreateInfo colourImageCI;
	colourImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	colourImageCI.pNext = NULL;
	colourImageCI.flags = 0; //Additional flags are in VkImageCreateFlagBits
	colourImageCI.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	colourImageCI.format = frameBufferFormat;
	colourImageCI.extent = VkExtent3D{ width, height, 1 };                   
	colourImageCI.mipLevels = 0;
	colourImageCI.arrayLayers = 1;
	colourImageCI.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	colourImageCI.tiling = VkImageTiling::VK_IMAGE_TILING_LINEAR; //TODO: dig into what these mean.
	colourImageCI.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	colourImageCI.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	colourImageCI.queueFamilyIndexCount = 0;
	colourImageCI.pQueueFamilyIndices = NULL; //Ignorable because sharingMode is not concurrent.
	colourImageCI.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	vkCreateImage(logicalDevice, &colourImageCI, NULL, &images[0]);

	//image view creation
	VkImageViewCreateInfo colourViewCI;
	colourViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	colourViewCI.pNext = NULL;
	colourViewCI.flags = 0;
	colourViewCI.image = images[0]; //image
	colourViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	colourViewCI.format = frameBufferFormat; //image format.
	colourViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
	colourViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
	colourViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
	colourViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
	colourViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	colourViewCI.subresourceRange.baseMipLevel = 0;
	colourViewCI.subresourceRange.levelCount = 1;
	colourViewCI.subresourceRange.baseArrayLayer = 0;
	colourViewCI.subresourceRange.layerCount = 1;

	vkCreateImageView(logicalDevice, &colourViewCI, NULL, &attachements[0]);


	// Depth buffer //
	if (useDepth)
	{
		//image creation
		VkImageCreateInfo depthImageCI;
		depthImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImageCI.pNext = NULL;
		depthImageCI.flags = 0; //Additional flags are in VkImageCreateFlagBits
		depthImageCI.imageType = VkImageType::VK_IMAGE_TYPE_2D;
		depthImageCI.format = VK_FORMAT_D16_UNORM;
		depthImageCI.extent = VkExtent3D{ width, height, 1 };
		depthImageCI.mipLevels = 0;
		depthImageCI.arrayLayers = 1;
		depthImageCI.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		depthImageCI.tiling = VkImageTiling::VK_IMAGE_TILING_LINEAR; //TODO: dig into what these mean.
		depthImageCI.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depthImageCI.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
		depthImageCI.queueFamilyIndexCount = 0;
		depthImageCI.pQueueFamilyIndices = NULL; //Ignorable because sharingMode is not concurrent.
		depthImageCI.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		vkCreateImage(logicalDevice, &depthImageCI, NULL, &images[1]);

		//image view creation
		VkImageViewCreateInfo depthViewCI;
		depthViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthViewCI.pNext = NULL;
		depthViewCI.flags = 0;
		depthViewCI.image = images[1]; //image
		depthViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthViewCI.format = VK_FORMAT_D16_UNORM; //image format.
		depthViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
		depthViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
		depthViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
		depthViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
		depthViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depthViewCI.subresourceRange.baseMipLevel = 0;
		depthViewCI.subresourceRange.levelCount = 1;
		depthViewCI.subresourceRange.baseArrayLayer = 0;
		depthViewCI.subresourceRange.layerCount = 1;

		vkCreateImageView(logicalDevice, &depthViewCI, NULL, &attachements[1]);
	}

	//frame buffer creation
	VkFramebufferCreateInfo frameBufferCI;
	frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCI.pNext = NULL;
	frameBufferCI.flags = 0;
	frameBufferCI.renderPass = renderPass;
	frameBufferCI.attachmentCount = (uint32_t)attachements.size();
	frameBufferCI.pAttachments = attachements.data();
	frameBufferCI.width = width;
	frameBufferCI.height = height;
	frameBufferCI.layers = 1;

	vkCreateFramebuffer(logicalDevice, &frameBufferCI, NULL, &frameBuffer);
}


FrameBuffer::~FrameBuffer()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//destroy image views
	for (size_t i = 0; i < attachements.size(); i++)
	{
		vkDestroyImageView(logicalDevice, attachements[i], NULL);
	}

	//destroy images
	for (size_t i = 0; i < images.size(); i++)
	{
		vkDestroyImage(logicalDevice, images[i], NULL);
	}
}
