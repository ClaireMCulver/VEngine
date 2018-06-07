#include "RenderTexture.h"



RenderTexture::RenderTexture(Image* renderedImage)
{
	this->renderedImage = renderedImage;

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
}


RenderTexture::~RenderTexture()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	vkDestroySampler(logicalDevice, textureSampler, NULL);
}