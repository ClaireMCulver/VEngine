#pragma once

#include <vector>

#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\quaternion.hpp"

#include "GraphicsDefs.h"
#include "VEngineDefs.h"
#include "vulkan\vulkan.h"

#include "GameObject.h"
class GameObject; 

#include "Graphics.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "DescriptorPool.h"

#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"

struct PipelineData
{
	VkPipeline pipeline; //Graphics Pipeline
	VkPipelineLayout layout; //Layout of descriptors in the layout
};

class Material
{
public:
	Material();
	~Material();

public:
	void SetOwner(GameObject* object) { owner = object; }
	inline GameObject* GetOwner() { return owner; }

	//Adds a new shader stage to the material
	void AddShader(Shader &newShader);

	//Finalizes the material pipeline on the GPU side.
	void FinalizeMaterial(VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayout pipelineLayout, VkPrimitiveTopology primitiveType = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

	//Binds the material
	void BindMaterial(glm::mat4 &mMatrix, glm::mat4 &vMatrix, glm::mat4 &vpMatrix, CommandBuffer *renderBuffer, VkPipelineLayout pipelineLayout);
	void BindPipeline(CommandBuffer &commandBuffer);

	//Returns the pipeline and pipeline layout handles
	PipelineData GetPipelineData() const { return pipelineData; }


	// Uniform updates //

	void UpdateDescriptorSet();
	void BindPerDrawUniforms(VkCommandBuffer vkRenderBuffer, VkPipelineLayout pipelineLayout);

	//Set the model, model view and model view projection matrices in the per draw uniform buffer.
	void SetDrawMatrices(glm::mat4 &modelMat, glm::mat4 &viewMat, glm::mat4 &viewProjoectionMat);

	//Updates the matrix uniform at the offset
	void SetUniform_Mat4x4(glm::mat4x4 &data, int offset);

	//Updates the integer uniform at the offset
	void SetUniform_Int32(int &data, int offset);

	//Updates the float uniform at the offset
	void SetUniform_Float32(float &data, int offset);

	void SetTexture(Texture& texture, int offset);
	void SetTexture(RenderTexture& texture, int offset);

	inline GPUBuffer* GetInstanceBuffer() { return instanceBuffer; }

private:
	GameObject * owner;

	//Pipeline handles
	PipelineData pipelineData;

	//Vertex binding and per-vertex attribute info
	std::vector<VkVertexInputBindingDescription> viBindings;//Binding of the vertices within the shaders
	std::vector<VkVertexInputAttributeDescription> viAttribs;//Bindings for the attributes of the vertices.

	//Pipeline information
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; //Stages in the shader pipeline

	//Descriptor Updating
	UniformBuffer* uniformBuffer = NULL;
	VkDescriptorSet uniformDescriptorSet;
	VkWriteDescriptorSet uniformWrite;
	std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkDescriptorImageInfo> textures;

	GPUBuffer* instanceBuffer;
	struct InstanceData
	{
		glm::vec3 position;
		glm::quat rotation;
	} instanceData;
};
