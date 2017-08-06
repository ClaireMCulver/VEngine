#include "Texture.h"



Texture::Texture(const char* fileName, int textureWidth, int textureHeight) : Image(textureWidth, textureHeight, VkFormat::VK_FORMAT_B8G8R8A8_UNORM, VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	// Image Sampler for textures //
	VkSamplerCreateInfo samplerCI;
	samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCI.pNext = NULL;
	samplerCI.flags = 0;
	samplerCI.magFilter = VkFilter::VK_FILTER_LINEAR;
	samplerCI.minFilter = VkFilter::VK_FILTER_LINEAR;
	samplerCI.mipmapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCI.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCI.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCI.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCI.mipLodBias = 0.0;
	samplerCI.anisotropyEnable = VK_FALSE;
	samplerCI.maxAnisotropy = 1;
	samplerCI.compareEnable = VK_FALSE;
	samplerCI.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCI.minLod = 0.0;
	samplerCI.maxLod = 0.0;
	samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	samplerCI.unnormalizedCoordinates = VK_FALSE;

	vkCreateSampler(logicalDevice, &samplerCI, NULL, &textureSampler);

	FIBITMAP* textureBMP;
	textureBMP = FreeImage_Load(FIF_PNG, fileName);

	//The size of the image in bytes = (number of bytes per pixel) times image dimensions.
	uint64_t BytesPerPixel = ((uint64_t)FreeImage_GetBPP(textureBMP) / 8),
		ImageWidth = FreeImage_GetWidth(textureBMP),
		ImageHeight = FreeImage_GetHeight(textureBMP);
	assert(textureHeight == ImageHeight && textureWidth == ImageWidth);

	uint64_t ImageSizeInBytes = BytesPerPixel * (textureHeight * textureWidth);

	CommandBuffer loadBuffer(CommandBufferType::Graphics, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	GPUBuffer storageBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT, FreeImage_GetBits(textureBMP), ImageSizeInBytes);

	VkBufferImageCopy imageRegion;
	imageRegion.bufferOffset = 0;
	imageRegion.bufferRowLength = 0;
	imageRegion.bufferImageHeight = 0;
	imageRegion.imageSubresource.aspectMask = vkImageAspect;
	imageRegion.imageSubresource.mipLevel = 0;
	imageRegion.imageSubresource.baseArrayLayer = 0;
	imageRegion.imageSubresource.layerCount = 1;
	imageRegion.imageOffset = { 0, 0, 0 };
	imageRegion.imageExtent = { (uint32_t)textureWidth, (uint32_t)textureHeight, 1 };

	loadBuffer.BeginRecording();

	ChangeImageLayout(loadBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	vkCmdCopyBufferToImage(loadBuffer.GetVKCommandBuffer(), storageBuffer.GetVKBuffer(), vkImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageRegion);

	ChangeImageLayout(loadBuffer, VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	loadBuffer.EndRecording();
	loadBuffer.SubmitBuffer();
	loadBuffer.WaitForCompletion();

	FreeImage_Unload(textureBMP);
}


Texture::~Texture()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	
	vkDestroySampler(logicalDevice, textureSampler, NULL);
}
