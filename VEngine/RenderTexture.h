#pragma once

#include "GPUBuffer.h"
#include "Image.h"
#include "FreeImage\FreeImage.h"

class RenderTexture : public Image
{
public:
	RenderTexture(Image* renderedImage);
	~RenderTexture();

public: // Get/Set
	VkSampler GetSampler() const { return textureSampler; }

public: // General Functions
	void CopyRenderedImage();

private:
	VkSampler textureSampler;

	Image* renderedImage;
	
};

