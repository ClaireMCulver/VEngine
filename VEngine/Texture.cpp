#include "Texture.h"



Texture::Texture(int imageWidth, int imageHeight, VkFormat imageFormat, VkImageUsageFlags usage, VkImageAspectFlags imageAspect) : Image(imageWidth, imageHeight, imageFormat, usage, imageAspect)
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
	samplerCI.maxAnisotropy = 1.0;
	samplerCI.compareEnable = VK_FALSE;
	samplerCI.compareOp = VK_COMPARE_OP_NEVER;
	samplerCI.minLod = 0.0;
	samplerCI.maxLod = 0.0;
	samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	samplerCI.unnormalizedCoordinates = VK_FALSE;

	vkCreateSampler(logicalDevice, &samplerCI, NULL, &textureSampler);
}


Texture::~Texture()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	
	vkDestroySampler(logicalDevice, textureSampler, NULL);
}
