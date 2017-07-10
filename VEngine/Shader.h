#pragma once

#include <vector>

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "glslang\Include\BaseTypes.h"
#include "glslang\Include\Common.h"
#include "glslang\Include\ShHandle.h"
#include "glslang\Include\intermediate.h"
#include "SPIRV\GlslangToSpv.h"

#include "Graphics.h"


class Shader
{
public:
	Shader(const char* filePath, VkShaderStageFlagBits shaderType);
	~Shader();

private:
	//Shader module
	VkShaderModule vkShaderModule;

	//Vertex binding and per-vertex attribute info
	VkVertexInputBindingDescription viBinding;//Binding of the vertices within the shaders
	std::vector<VkVertexInputAttributeDescription> viAttribs;//Bindings for the attributes of the vertices.

	VkDescriptorSetLayout resourceSetLayout;

private:
	//TODO: This currently does not take in a file path, but the actual shader text. Fix that.
	bool LoadShaderFromFile(const char* filePath, VkShaderStageFlagBits shaderType);

	//Creates vertex and uv bindings for the shader. TODO: ADD NORMALS, TODO WAY DOWN THE LINE: Automatic shader binding and uniform reader.
	bool CreateVertexBindings();

	//Finds the number of uniforms in the shader and creates relevant bindings for them.
	bool CreateResourceSetLayout(const std::string* fileData, VkShaderStageFlagBits shaderType);

private:
	bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, const char* shaderName, std::vector<unsigned int> &spirv);
	void init_resources(TBuiltInResource &Resources);
	EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type);
	bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);


};

