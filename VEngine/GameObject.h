#pragma once

//GLM


//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Geometry.h"
#include "Material.h"
class Material;
#include "Renderer.scr"
#include "CommandBuffer.h"
#include "GPUBuffer.h"

#include "ObjectManager.h" 
class ObjectManager;

#include "Component.h"
#include "Transform.h"

#include <unordered_map>

class GameObject
{
public:
	GameObject(Geometry *mesh, Material *material);
	~GameObject();

	// Get/Set //
	inline Transform* GetTransform() { return transform; }
	inline Geometry* GetGeometry() { return geometry; }
	inline Material* GetMaterial() { return material; }
	inline Renderer* GetRenderer() { return renderer; }

	void SetRenderer(Renderer* givenRenderer);

	// Updates //

	void Update();

	// Components //

	//Should be a pointer to a heap component. Will be deleted by the GameObject.
	void AddComponent(Component* component);

	//Returns nullptr if there is no component of that type;
	template<typename T>
	T* GetComponent()
	{
		if (components.count(&typeid(T)) != 0)
		{
			return static_cast<T*>(components[&typeid(T)]);
		}
		else
		{
			return nullptr;
		}
	}
		
private:
	Transform* transform;

	Geometry* geometry = nullptr;
	Material* material = nullptr;
	Renderer* renderer = nullptr;

	glm::mat4x4 MVPMatrix;

	std::unordered_map<const std::type_info*, Component*> components;
};

