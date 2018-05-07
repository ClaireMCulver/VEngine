#pragma once

#include <vector>

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "shaderc\shaderc.hpp"

#include "Graphics.h"


class Shader
{
public:
	Shader(const char* filePath, VkShaderStageFlagBits shaderType);
	~Shader();

	VkShaderModule GetVKShaderModule() const { return vkShaderModule; }
	VkShaderStageFlags GetVKShaderStage() const { return vkShaderType; }

private:
	//Shader module
	VkShaderModule vkShaderModule;
	VkShaderStageFlags vkShaderType;

private:
	//Loads the file and places the text within the given string.
	void LoadShaderFromFile(const char* filePath, std::string &fileText);

	//Takes the text and creates a shader module
	bool CreateShaderModule(const char* fileText, VkShaderStageFlagBits shaderType);

private:
	std::vector<uint32_t> GLSLtoSPV(const std::string& source_name, VkShaderStageFlagBits stageFlagBits, const std::string& source, bool optimize = false);
	//bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, const char* shaderName, std::vector<unsigned int> &spirv);
	//void init_resources(TBuiltInResource &Resources);
	//EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type);
	bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
};

