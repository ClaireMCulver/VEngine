#pragma once

//GLM
#include "glm\mat4x4.hpp"

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Geometry.h"
#include "Material.h"
#include "CommandBuffer.h"
#include "GPUBuffer.h"
#include "DescriptorPool.h"
#include "Texture.h"

class RenderableObject
{
public:
	RenderableObject(Geometry *mesh, Material *material);
	~RenderableObject();

	void Draw(CommandBuffer &commandBuffer);

	// Uniform updates //

	//Updates the uniform in uniformSet at binding
	void SetUniform_Mat4x4(glm::mat4x4 &data, int uniformSet, int binding);

	void SetTexture(Texture& texture, int uniformSet, int binding);

private:
	Geometry* geometry;
	Material* material;

	glm::mat4x4 MVPMatrix;

	std::vector<VkDescriptorSet> uniforms;

	GPUBuffer* uniformBuffer = NULL;
	VkWriteDescriptorSet uniformWrite;
};

