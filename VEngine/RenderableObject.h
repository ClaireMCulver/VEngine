#pragma once

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Geometry.h"
#include "Material.h"
#include "CommandBuffer.h"

class RenderableObject
{
public:
	RenderableObject(Geometry *mesh, Material *material);
	~RenderableObject();

	void Draw(CommandBuffer &commandBuffer);

private:
	Geometry* geometry;
	Material* material;

	std::vector<VkDescriptorSet> uniforms;
};

