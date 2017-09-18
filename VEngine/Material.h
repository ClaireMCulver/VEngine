#pragma once

#include <vector>

#include "GraphicsDefs.h"
#include "VEngineDefs.h"
#include "vulkan\vulkan.h"

#include "Graphics.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "Texture.h"

#include "glm\glm.hpp"

struct PipelineData
{
	VkPipeline pipeline; //Graphics Pipeline
};

class Material
{
public:
	Material();
	~Material();

public:
	//Adds a new shader stage to the material
	void AddShader(Shader &newShader);

	//Finalizes the material pipeline on the GPU side.
	void FinalizeMaterial(VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayout pipelineLayout);


	//Binds the pipeline
	void BindPipeline(CommandBuffer &commandBuffer);

	//Returns the pipeline and pipeline layout handles
	PipelineData GetPipelineData() const { return pipelineData; }

	void UpdateDescriptorSet(VkDescriptorSet &descriptorSet);

	// Uniform updates //

	//Updates the uniform in uniformSet at binding
	void SetUniform_Mat4x4(glm::mat4x4 &data, int offset);

	void SetTexture(Texture& texture, int offset);

private:
	//Pipeline handles
	PipelineData pipelineData;

	//Vertex binding and per-vertex attribute info
	VkVertexInputBindingDescription viBinding;//Binding of the vertices within the shaders
	std::vector<VkVertexInputAttributeDescription> viAttribs;//Bindings for the attributes of the vertices.

	//Pipeline information
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; //Stages in the shader pipeline

	//Descriptor Updating
	UniformBuffer* uniformBuffer = NULL;
	VkWriteDescriptorSet uniformWrite;

	std::vector<Texture*> textures;
};
