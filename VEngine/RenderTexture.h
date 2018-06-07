#pragma once

#include "GPUBuffer.h"
#include "Image.h"
#include "FreeImage\FreeImage.h"

class RenderTexture
{
public:
	RenderTexture(Image* renderedImage);
	~RenderTexture();

public: // Get/Set
	VkSampler GetSampler() const { return textureSampler; }
	VkImageView GetImageView() const { return renderedImage->GetImageView(); }
	VkImageLayout GetImageLayout() const { return renderedImage->GetImageLayout(); }

public: // General Functions

private:
	VkSampler textureSampler;

	Image* renderedImage;
	
};

