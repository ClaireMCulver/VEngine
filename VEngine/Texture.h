#pragma once


#include "GPUBuffer.h"
#include "Image.h"
#include "FreeImage\FreeImage.h"

class Texture :	public Image
{
public:
	//Allocate a texture for filling later.
	Texture(int textureWidth, int textureHeight);

	//Create texture from existing Image object
	Texture(Image* image);

	//Load a texture directly from a file
	Texture(const char* fileName, int textureWidth, int textureHeight);
	~Texture();

public: // Get/Set
	VkSampler GetSampler() const { return textureSampler; }

public: // General Functions
	void CopyRenderedImage(Image* existingimage);

private: // private general functions
	void InitializeSampler(const VkDevice &logicalDevice);

private:
	VkSampler textureSampler;
};

