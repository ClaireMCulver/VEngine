#pragma once

//GLM
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\quaternion.hpp"

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Geometry.h"
#include "Material.h"
#include "Renderer.cpp"
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
	inline Transform* GetTransform() { return transform; }
	inline Geometry* GetGeometry() { return geometry; }
	inline Material* GetMaterial() { return material; }
	inline Renderer* GetRenderer() { return renderer; }
	inline GPUBuffer* GetInstanceBuffer() { return instanceBuffer; }

	inline void SetRenderer(Renderer* givenRenderer) { renderer = givenRenderer; }

	// Updates //

	void Draw(CommandBuffer &commandBuffer, VkPipelineLayout pipelineLayout);

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

	void UpdateDescriptorSet();
	void BindPerDrawUniforms(VkCommandBuffer vkRenderBuffer, VkPipelineLayout pipelineLayout);

	// Uniform updates //

	//Set the model, model view and model view projection matrices in the per draw uniform buffer.
	void SetDrawMatrices(glm::mat4 &modelMat, glm::mat4 &viewMat, glm::mat4 &viewProjoectionMat);

	//Updates the matrix uniform at the offset
	void SetUniform_Mat4x4(glm::mat4x4 &data, int offset);

	//Updates the integer uniform at the offset
	void SetUniform_Int32(int &data, int offset);

	//Updates the float uniform at the offset
	void SetUniform_Float32(float &data, int offset);

	void SetTexture(Texture& texture, int offset);

private:
	Transform* transform;

	Geometry* geometry = nullptr;
	Material* material = nullptr;
	Renderer* renderer = nullptr;

	glm::mat4x4 MVPMatrix;

	std::unordered_map<const std::type_info*, Component*> components;

	//Descriptor Updating
	UniformBuffer* uniformBuffer = NULL;
	VkDescriptorSet uniformDescriptorSet;
	VkWriteDescriptorSet uniformWrite;
	std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
	VkDescriptorSetLayout descriptorSetLayout;

	GPUBuffer* instanceBuffer;
	struct InstanceData 
	{
		glm::vec3 position;
		glm::quat rotation;
	} instanceData;

	std::vector<Texture*> textures;
};

