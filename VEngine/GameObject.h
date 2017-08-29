#pragma once

//GLM
#include "glm\glm.hpp"

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

#include "Component.h"
#include "Transform.h"

class GameObject
{
public:
	GameObject(Geometry *mesh, Material *material);
	~GameObject();

	// Get/Set //
	Transform* getTransform() { return transform; }

	// Updates //

	void Draw(CommandBuffer &commandBuffer);

	void Update();

	// Components //
	void AddComponent(Component* component);

	// Uniform updates //

	//Updates the uniform in uniformSet at binding
	void SetUniform_Mat4x4(glm::mat4x4 &data, int uniformSet, int binding);

	void SetTexture(Texture& texture, int uniformSet, int binding);

private:
	Transform* transform;

	Geometry* geometry;
	Material* material;

	glm::mat4x4 MVPMatrix;

	std::vector<VkDescriptorSet> uniforms;

	GPUBuffer* uniformBuffer = NULL;
	VkWriteDescriptorSet uniformWrite;

	std::vector<Component*> components;
};

