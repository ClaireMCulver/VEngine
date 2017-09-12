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
	Transform* GetTransform() { return transform; }
	Material* GetMaterial() { return material; }

	// Updates //

	void Draw(CommandBuffer &commandBuffer);

	void Update();

	// Components //
	void AddComponent(Component* component);



private:
	Transform* transform;

	Geometry* geometry;
	Material* material;

	glm::mat4x4 MVPMatrix;

	std::vector<Component*> components;
};

