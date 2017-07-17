#pragma once

#include <vector>

#include "GraphicsDefs.h"
#include "vulkan\vulkan.h"

#include "Graphics.h"
#include "Shader.h"
#include "RenderPass.h"

struct PipelineData
{
	VkPipeline pipeline; //Graphics Pipeline
	VkPipelineLayout pipelineLayout; //Layout of the pipeline
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
	void FinalizeMaterial(RenderPass &renderPass);


	//Binds the pipeline
	void BindPipeline(CommandBuffer &commandBuffer);

	//Returns the pipeline and pipeline layout handles
	PipelineData GetPipelineData() const { return pipelineData; }
	std::vector<VkDescriptorSetLayout> GetDescriptorLayout() const { return layoutDescriptors; }

private:
	//Pipeline handles
	PipelineData pipelineData;

	//Vertex binding and per-vertex attribute info
	VkVertexInputBindingDescription viBinding;//Binding of the vertices within the shaders
	std::vector<VkVertexInputAttributeDescription> viAttribs;//Bindings for the attributes of the vertices.

	//Pipeline information
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; //Stages in the shader pipeline
	std::vector<VkDescriptorSetLayout> layoutDescriptors; //uniform layout bindings for the shaders.
};
