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
	//Adds a new shader stage to the material
	void AddShader(Shader &newShader);

	//Finalizes the material pipeline on the GPU side.
	void FinalizeMaterial(VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayout pipelineLayout, VkPrimitiveTopology primitiveType);



	//Binds the pipeline
	void BindPipeline(CommandBuffer &commandBuffer);

	//Returns the pipeline and pipeline layout handles
	PipelineData GetPipelineData() const { return pipelineData; }

private:
	//Pipeline handles
	PipelineData pipelineData;

	//Vertex binding and per-vertex attribute info
	std::vector<VkVertexInputBindingDescription> viBindings;//Binding of the vertices within the shaders
	std::vector<VkVertexInputAttributeDescription> viAttribs;//Bindings for the attributes of the vertices.

	//Pipeline information
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; //Stages in the shader pipeline
};
