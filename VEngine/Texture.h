#pragma once
#include "Image.h"
class Texture :
	public Image
{
public:
	Texture(int imageWidth, int imageHeight, VkFormat imageFormat, VkImageUsageFlags usage, VkImageAspectFlags imageAspect);
	~Texture();

	VkSampler GetSampler() const { return textureSampler; }

private:
	VkSampler textureSampler;
};

