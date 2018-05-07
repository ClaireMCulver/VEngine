#include "Shader.h"



Shader::Shader(const char* filePath, VkShaderStageFlagBits shaderType)
{
	vkShaderType = shaderType;

	std::string fileText;

	LoadShaderFromFile(filePath, fileText);

	CreateShaderModule(fileText.data(), shaderType);
}


Shader::~Shader()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	
	vkDestroyShaderModule(logicalDevice, vkShaderModule, NULL);
}

void Shader::LoadShaderFromFile(const char* filePath, std::string &fileText)
{
	int result = 0;

	FILE* shaderFile;
	result = fopen_s(&shaderFile, filePath, "r");
	assert(result == 0);

	fseek(shaderFile, 0, SEEK_END);
	long fileSize = ftell(shaderFile);
	rewind(shaderFile);

	fileText.resize((size_t)fileSize);
	
	fread((void*)fileText.data(), 1, fileSize, shaderFile);

	fclose(shaderFile);
}

bool Shader::CreateShaderModule(const char* fileText, VkShaderStageFlagBits shaderType)
{
	std::vector<unsigned int> shaderSPIRV = GLSLtoSPV("NewShader", shaderType, fileText, true);

	VkPipelineShaderStageCreateInfo newShader;
	newShader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	newShader.pNext = NULL;
	newShader.flags = 0;
	newShader.stage = shaderType;
	newShader.pSpecializationInfo = NULL;
	newShader.pName = "main";

	VkShaderModuleCreateInfo moduleCI;
	moduleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCI.pNext = NULL;
	moduleCI.flags = 0;
	moduleCI.codeSize = shaderSPIRV.size() * sizeof(unsigned int);
	moduleCI.pCode = shaderSPIRV.data();

	//Actually create the shader vulkan side:
	VkResult result = vkCreateShaderModule(GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice(), &moduleCI, NULL, &vkShaderModule);
	assert(result == VK_SUCCESS);

	return true;
}

std::vector<uint32_t> Shader::GLSLtoSPV(const std::string& source_name, VkShaderStageFlagBits stageFlagBits, const std::string& source, bool optimize)
{
	// Find shader stage enum
	shaderc_shader_kind kind;
	switch (stageFlagBits)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		kind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
		break;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		kind = shaderc_shader_kind::shaderc_glsl_tess_control_shader;
		break;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		kind = shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;
		break;
	case VK_SHADER_STAGE_GEOMETRY_BIT:
		kind = shaderc_shader_kind::shaderc_glsl_geometry_shader;
		break;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		kind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
		break;
	case VK_SHADER_STAGE_COMPUTE_BIT:
		kind = shaderc_shader_kind::shaderc_glsl_compute_shader;
		break;
	}

	// Compiler options
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// define example
	//options.AddMacroDefinition("MY_DEFINE", "1");

	if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_zero);

	// Compile
	shaderc::SpvCompilationResult module =
		compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) 
	{
		printf("%s", module.GetErrorMessage().c_str());
		return std::vector<uint32_t>();
	}

	return { module.cbegin(), module.cend() };
}

bool Shader::memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

