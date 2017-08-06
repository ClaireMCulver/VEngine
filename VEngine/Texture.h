#pragma once


#include "GPUBuffer.h"
#include "Image.h"
#include "FreeImage\FreeImage.h"

class Texture :
	public Image
{
public:
	Texture(const char* fileName, int textureWidth, int textureHeight);
	~Texture();

	VkSampler GetSampler() const { return textureSampler; }

private:
	VkSampler textureSampler;
};

