#pragma once

#include <vector>

#include "GraphicsDefs.h"
#include "vulkan\vulkan.h"

#include "glslang\Include\BaseTypes.h"
#include "glslang\Include\Common.h"
#include "glslang\Include\ShHandle.h"
#include "glslang\Include\intermediate.h"
#include "SPIRV\GlslangToSpv.h"

#include "GraphicsSystem.h"

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
	bool LoadShaderFromFile(const char* filePath, VkShaderStageFlagBits shaderType);

	PipelineData GetPipelineData() const { return pipelineData; }

	bool PrepMaterial();

private:
	bool CreateBindings();
	bool CreatePipelineLayout();
	bool CreateRenderPass();
	bool CreatePipeline();
	void InitializeUniformBuffer();
	bool CreateDescriptors();

	bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, const char* shaderName, std::vector<unsigned int> &spirv);
	void init_resources(TBuiltInResource &Resources);
	EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type);
	bool Material::memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

private:
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages; //Stages in the shader pipeline
	VkVertexInputBindingDescription viBinding;//Binding of the vertices within the shaders
	VkVertexInputAttributeDescription viAttribs[2];//Bindings for the attributes of the vertices.
	std::vector<VkDescriptorSetLayout> layoutDescriptors; //uniform layout bindings for the shaders.

	VkRenderPass renderPass;

	//Uniforms. They're here because I don't have another place to put them. They might belong here actually.
	glm::mat4x4 projectionMat;
	glm::mat4x4 viewMat;
	glm::mat4x4 modelMat;
	glm::mat4x4 clipMat;
	
	glm::mat4x4 mvpMat;
	UniformData uniformData; //Buffer for all uniforms. For now, just the mvp matrix.

	PipelineData pipelineData;
	//std::vector<VkDescriptorSet> descriptors; //Describes the uniform data buffer to vulkan.
	//VkPipeline pipeline; //Graphics Pipeline
	//VkPipelineLayout pipelineLayout; //Layout of the pipeline
};
