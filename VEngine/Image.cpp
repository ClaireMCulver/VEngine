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
	ImageCI.usage = usage;
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

	//Bind it in order to create the image view.
	result = vkBindImageMemory(logicalDevice, vkImage, vkDeviceMemory, 0);
	assert(result == VK_SUCCESS);


	// Image View Creation //

	//clear values for the image
	VkClearValue clearColourValue;
	clearColourValue.color.float32[0] = 0.0f;
	clearColourValue.color.float32[1] = 0.0f;
	clearColourValue.color.float32[2] = 0.0f;
	clearColourValue.color.float32[3] = 0.0f;
	clearColourValue.depthStencil.depth = 1.0;
	clearColourValue.depthStencil.stencil = 0;

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
}


Image::~Image()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	vkDestroyImageView(logicalDevice, vkImageView, NULL);

	vkFreeMemory(logicalDevice, vkDeviceMemory, NULL);

	vkDestroyImage(logicalDevice, vkImage, NULL);
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