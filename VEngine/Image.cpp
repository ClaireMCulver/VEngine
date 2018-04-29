#include "Image.h"



Image::Image(int imageWidth, int imageHeight, VkFormat imageFormat, VkImageUsageFlags usage, VkImageAspectFlags imageAspect)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	VkResult result;

	// Image Creation //
	VkImageCreateInfo ImageCI;
	ImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageCI.pNext = NULL;
	ImageCI.flags = 0; //Additional flags are in VkImageCreateFlagBits
	ImageCI.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	ImageCI.format = imageFormat;
	ImageCI.extent = VkExtent3D{ (uint32_t)imageWidth, (uint32_t)imageHeight, 1 };
	ImageCI.mipLevels = 1;
	ImageCI.arrayLayers = 1;
	ImageCI.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	ImageCI.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	ImageCI.usage = usage | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	ImageCI.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	ImageCI.queueFamilyIndexCount = 0;
	ImageCI.pQueueFamilyIndices = NULL; //Ignorable because sharingMode is not concurrent.
	ImageCI.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

	result = vkCreateImage(logicalDevice, &ImageCI, NULL, &vkImage);
	assert(result == VK_SUCCESS);


	// Memory Allocation //

	//Get the memory requirements for the image
	VkMemoryRequirements imageMemoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice, vkImage, &imageMemoryRequirements);

	//Allocate the memory
	VkMemoryAllocateInfo imageMemoryCI;
	imageMemoryCI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	imageMemoryCI.pNext = NULL;
	imageMemoryCI.allocationSize = imageMemoryRequirements.size;
	imageMemoryCI.memoryTypeIndex = FindMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, imageMemoryRequirements.memoryTypeBits);
	result = vkAllocateMemory(logicalDevice, &imageMemoryCI, NULL, &vkDeviceMemory);
	assert(result == VK_SUCCESS);

	//Bind the image to the memory object
	result = vkBindImageMemory(logicalDevice, vkImage, vkDeviceMemory, 0);
	assert(result == VK_SUCCESS);


	// Image View Creation //

	//clear values for the image
	if ((usage & VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) == VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	{
		vkClearColour.depthStencil.depth = 1.0;
		vkClearColour.depthStencil.stencil = 0;
	}
	else
	{
		vkClearColour.color.float32[0] = 0.0f;
		vkClearColour.color.float32[1] = 0.0f;
		vkClearColour.color.float32[2] = 0.0f;
		vkClearColour.color.float32[3] = 0.0f;
	}

	//image view create info
	VkImageViewCreateInfo imageViewCI;
	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCI.pNext = NULL;
	imageViewCI.flags = 0;
	imageViewCI.image = vkImage; //image
	imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCI.format = imageFormat; //image format.
	imageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
	imageViewCI.subresourceRange.aspectMask = imageAspect;
	imageViewCI.subresourceRange.baseMipLevel = 0;
	imageViewCI.subresourceRange.levelCount = 1;
	imageViewCI.subresourceRange.baseArrayLayer = 0;
	imageViewCI.subresourceRange.layerCount = 1;

	result = vkCreateImageView(logicalDevice, &imageViewCI, NULL, &vkImageView);
	assert(result == VK_SUCCESS);

	imageSize.x = (float)imageWidth;
	imageSize.y = (float)imageHeight;
	vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vkImageAspect = imageAspect;
}

Image::~Image()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	vkDestroyImageView(logicalDevice, vkImageView, NULL);

	vkFreeMemory(logicalDevice, vkDeviceMemory, NULL);

	vkDestroyImage(logicalDevice, vkImage, NULL);
}

void Image::ChangeImageLayout(VkImageLayout newLayout) 
{
	ImageAccessData accessData = FetchImageAccessAndStage(newLayout);

	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = vkImageAspect;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	VkImageMemoryBarrier imageMemoryBarrier;
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = NULL; 
	imageMemoryBarrier.srcAccessMask = accessData.srcMask;
	imageMemoryBarrier.dstAccessMask = accessData.dstMask; //TODO: deal with pipeline stages. https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#synchronization-pipeline-stages-masks
	imageMemoryBarrier.oldLayout = vkImageLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = vkImage;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	CommandBuffer layoutBuffer(CommandBufferType::Graphics, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	layoutBuffer.BeginRecording();
	vkCmdPipelineBarrier(
		layoutBuffer.GetVKCommandBuffer(),
		accessData.srcStageFlags,
		accessData.dstStageFlags,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
	layoutBuffer.EndRecording();
	layoutBuffer.SubmitBuffer();

	vkImageLayout = newLayout;
}

void Image::ChangeImageLayout(CommandBuffer& commandBuffer, VkImageLayout newLayout)
{
	
	ImageAccessData accessData = FetchImageAccessAndStage(newLayout);

	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = vkImageAspect;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	VkImageMemoryBarrier imageMemoryBarrier;
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = NULL;
	imageMemoryBarrier.srcAccessMask = accessData.srcMask;
	imageMemoryBarrier.dstAccessMask = accessData.dstMask;
	imageMemoryBarrier.oldLayout = vkImageLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = vkImage;
	imageMemoryBarrier.subresourceRange = subresourceRange;


	vkCmdPipelineBarrier(
		commandBuffer.GetVKCommandBuffer(),
		accessData.srcStageFlags,
		accessData.dstStageFlags,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);

	vkImageLayout = newLayout;
}

uint32_t Image::FindMemoryType(uint32_t typeBits, VkFlags requirements_mask)
{
	const VkPhysicalDeviceMemoryProperties memory_properties = GraphicsSystem::GetSingleton()->GetPhysicalDevice()->GetPhysicalDeviceMemoryPropertiess();

	uint32_t typeIndex = 0;

	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				typeIndex = i;
				return typeIndex;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return typeIndex;
}

void Image::CmdClearImage(CommandBuffer &commandBuffer)
{
	VkImageSubresourceRange imageResource;
	imageResource.aspectMask = vkImageAspect;
	imageResource.baseMipLevel = 0;
	imageResource.levelCount = 1;
	imageResource.baseArrayLayer = 0;
	imageResource.layerCount = 1;

	VkImageLayout currentLayout = vkImageLayout;

	ChangeImageLayout(commandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	if (vkImageAspect == VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)
	{
		vkCmdClearColorImage(commandBuffer.GetVKCommandBuffer(), vkImage, vkImageLayout, &vkClearColour.color, 1, &imageResource);
	}
	else
	{
		vkCmdClearDepthStencilImage(commandBuffer.GetVKCommandBuffer(), vkImage, vkImageLayout, &vkClearColour.depthStencil, 1, &imageResource);
	}

	ChangeImageLayout(commandBuffer, currentLayout);
}

void Image::CmdBlitImage(CommandBuffer & commandBuffer, Image * sourceImage, Image * destinationImage, VkFilter filter)
{
	VkImageBlit imageResolve;
	imageResolve.srcSubresource;
	imageResolve.srcSubresource.aspectMask = sourceImage->GetImageAspect();
	imageResolve.srcSubresource.mipLevel = 0;
	imageResolve.srcSubresource.baseArrayLayer = 0;
	imageResolve.srcSubresource.layerCount = 1;
	imageResolve.srcOffsets[0] = { 0, 0, 0 };
	imageResolve.srcOffsets[1] = { (int32_t)sourceImage->GetImageSize().x, (int32_t)sourceImage->GetImageSize().y, 1 };

	imageResolve.dstSubresource;
	imageResolve.dstSubresource.aspectMask = destinationImage->GetImageAspect();;
	imageResolve.dstSubresource.mipLevel = 0;;
	imageResolve.dstSubresource.baseArrayLayer = 0;;
	imageResolve.dstSubresource.layerCount = 1;;
	imageResolve.dstOffsets[0] = { 0, 0, 0 };
	imageResolve.dstOffsets[1] = { (int32_t)destinationImage->GetImageSize().x, (int32_t)destinationImage->GetImageSize().y, 1 };

	//Blit the image to the 28x28 image
	vkCmdBlitImage(commandBuffer.GetVKCommandBuffer(), sourceImage->GetImage(), sourceImage->GetImageLayout(), destinationImage->GetImage(), destinationImage->GetImageLayout(), 1, &imageResolve, filter);
}

Image::ImageAccessData Image::FetchImageAccessAndStage(VkImageLayout currentLayout, VkImageLayout newLayout)
{
	Image::ImageAccessData accessData;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (currentLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		accessData.srcMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		accessData.srcMask = VK_ACCESS_HOST_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_HOST_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		accessData.srcMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		accessData.srcMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source 
		// Make sure any reads from the image have been finished
		accessData.srcMask = VK_ACCESS_TRANSFER_READ_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		accessData.srcMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		accessData.srcMask = VK_ACCESS_SHADER_READ_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		accessData.dstMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;

		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from and writes to the image have been finished
		accessData.dstMask = VK_ACCESS_TRANSFER_READ_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		accessData.dstMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		accessData.dstMask = accessData.dstMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (accessData.srcMask == 0)
		{
			accessData.srcMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		accessData.dstMask = VK_ACCESS_SHADER_READ_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		accessData.dstMask = VK_ACCESS_MEMORY_READ_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;
	}

	return accessData;
}

Image::ImageAccessData Image::FetchImageAccessAndStage(VkImageLayout newLayout)
{
	ImageAccessData accessData;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (vkImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		accessData.srcMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		accessData.srcMask = VK_ACCESS_HOST_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_HOST_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		accessData.srcMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		accessData.srcMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source 
		// Make sure any reads from the image have been finished
		accessData.srcMask = VK_ACCESS_TRANSFER_READ_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		accessData.srcMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		accessData.srcMask = VK_ACCESS_SHADER_READ_BIT;
		accessData.srcStageFlags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		accessData.dstMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;

		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from and writes to the image have been finished
		accessData.dstMask = VK_ACCESS_TRANSFER_READ_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		accessData.dstMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		accessData.dstMask = accessData.dstMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (accessData.srcMask == 0)
		{
			accessData.srcMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		accessData.dstMask = VK_ACCESS_SHADER_READ_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		accessData.dstMask = VK_ACCESS_MEMORY_READ_BIT;
		accessData.dstStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;
	}

	return accessData;
}