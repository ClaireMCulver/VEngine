#pragma once

#include <vector>

#include "GraphicsDefs.h"
#include "vulkan\vulkan.h"

#include "Shader.h"
#include "RenderPass.h"

struct PipelineData
{
	VkPipeline pipeline; //Graphics Pipeline
	VkPipelineLayout pipelineLayout; //Layout of the pipeline
	std::vector<VkDescriptorSet> descriptors; //Describes the uniform data buffer to vulkan.
};

class Material
{
public:
	Material();
	~Material();

public:
	void AddShader(Shader newShader);

	PipelineData GetPipelineData() const { return pipelineData; }
private:


private:
	PipelineData pipelineData;
	
	//Vertex binding and per-vertex attribute info
	VkVertexInputBindingDescription viBinding;//Binding of the vertices within the shaders
	std::vector<VkVertexInputAttributeDescription> viAttribs;//Bindings for the attributes of the vertices.

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; //Stages in the shader pipeline
	std::vector<VkDescriptorSetLayout> layoutDescriptors; //uniform layout bindings for the shaders.

	//std::vector<VkDescriptorSet> descriptors; //Describes the uniform data buffer to vulkan.
	//VkPipeline pipeline; //Graphics Pipeline
	//VkPipelineLayout pipelineLayout; //Layout of the pipeline
};
