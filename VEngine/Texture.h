#pragma once


#include "GPUBuffer.h"
#include "Image.h"
#include "FreeImage\FreeImage.h"

class Texture :	public Image
{
public:
	Texture(int textureWidth, int textureHeight);

	//Load a texture directly from a file
	Texture(const char* fileName, int textureWidth, int textureHeight);
	~Texture();

public: // Get/Set
	VkSampler GetSampler() const { return textureSampler; }

public: // General Functions
	void CopyRenderedImage();

private: // private general functions
	void InitializeSampler(const VkDevice &logicalDevice);

private:
	VkSampler textureSampler;
};

