#pragma once

#include "GraphicsSystem.h"

#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

namespace Graphics
{
	void BlitImage(VkImage source, VkImage destination)
	{
		VkImageBlit region;
		region.srcSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		region.srcSubresource.mipLevel = 0;
		region.srcSubresource.baseArrayLayer = 0;
		region.srcSubresource.layerCount = 1;
		region.srcOffsets[0] = { 0, 0, 0 };
		region.srcOffsets[1] = { 0, 0, 1 };
		region.dstSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		region.dstSubresource.mipLevel = 0;
		region.dstSubresource.baseArrayLayer = 0;
		region.dstSubresource.layerCount = 1;
		region.dstOffsets[0] = { 0, 0, 0 };
		region.dstOffsets[1] = { 0, 0, 1 };

		vkCmdBlitImage(, source, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, destination, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VkFilter::VK_FILTER_LINEAR);
	}
}