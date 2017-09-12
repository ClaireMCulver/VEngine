#pragma once

#include <vector>

#include "GraphicsDefs.h"
#include "vulkan\vulkan.h"

#include "Graphics.h"
#include "Shader.h"

#include "DescriptorPool.h"
#include "GPUBuffer.h"
#include "Texture.h"

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
	void FinalizeMaterial(VkRenderPass renderPass);


	//Binds the pipeline
	void BindPipeline(CommandBuffer &commandBuffer);

	//Returns the pipeline and pipeline layout handles
	PipelineData GetPipelineData() const { return pipelineData; }
	std::vector<VkDescriptorSetLayout> GetDescriptorLayout() const { return layoutDescriptors; }


	// Uniform updates //

	//Updates the uniform in uniformSet at binding
	void SetUniform_Mat4x4(glm::mat4x4 &data, int uniformSet, int binding);

	void SetTexture(Texture& texture, int uniformSet, int binding);

private:
	//Pipeline handles
	PipelineData pipelineData;

	//Vertex binding and per-vertex attribute info
	VkVertexInputBindingDescription viBinding;//Binding of the vertices within the shaders
	std::vector<VkVertexInputAttributeDescription> viAttribs;//Bindings for the attributes of the vertices.

	//Pipeline information
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; //Stages in the shader pipeline
	std::vector<VkDescriptorSetLayout> layoutDescriptors; //uniform layout bindings for the shaders.

	//Uniform writing
	std::vector<std::vector<GPUBuffer*>> uniformBuffers;
	std::vector<VkDescriptorSet> uniformSets; //UniformDescriptions
	VkWriteDescriptorSet uniformWrite;
};
