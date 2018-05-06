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

	//Convert from glsl to spir-v
	//glslang::InitializeProcess();
	//TBuiltInResource resources;
	//init_resources(resources);
	//GLSLtoSPV(shaderType, fileText, "NewShader", shaderSPIRV);
	//glslang::FinalizeProcess(); //This will either go here or at the end of the function, after loading the shader module. Probably just here.

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

//bool Shader::GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char *pshader, const char* shaderName, std::vector<unsigned int> &spirv)
//{
//	EShLanguage stage = FindLanguage(shader_type);
//	glslang::TShader shader(stage);
//	glslang::TProgram program;
//	const char *shaderStrings[1];
//	TBuiltInResource Resources;
//	init_resources(Resources);
//
//	std::string processedString;
//
//	// Enable SPIR-V and Vulkan rules when parsing GLSL
//	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
//
//	shaderStrings[0] = pshader;
//	shader.setStrings(shaderStrings, 1);
//
//
//	if (!shader.parse(&Resources, 100, false, messages)) {
//		puts(shader.getInfoLog());
//		puts(shader.getInfoDebugLog());
//		fflush(stdout);
//		return false;  // something didn't work
//	}
//
//	program.addShader(&shader);
//
//	//
//	// Program-level processing...
//	//
//
//	if (!program.link(messages)) {
//		puts(shader.getInfoLog());
//		puts(shader.getInfoDebugLog());
//		fflush(stdout);
//		return false;
//	}
//
//	glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
//
//	return true;
//}
//
//void Shader::init_resources(TBuiltInResource &Resources) {
//	Resources.maxLights = 32;
//	Resources.maxClipPlanes = 6;
//	Resources.maxTextureUnits = 32;
//	Resources.maxTextureCoords = 32;
//	Resources.maxVertexAttribs = 64;
//	Resources.maxVertexUniformComponents = 4096;
//	Resources.maxVaryingFloats = 64;
//	Resources.maxVertexTextureImageUnits = 32;
//	Resources.maxCombinedTextureImageUnits = 80;
//	Resources.maxTextureImageUnits = 32;
//	Resources.maxFragmentUniformComponents = 4096;
//	Resources.maxDrawBuffers = 32;
//	Resources.maxVertexUniformVectors = 128;
//	Resources.maxVaryingVectors = 8;
//	Resources.maxFragmentUniformVectors = 16;
//	Resources.maxVertexOutputVectors = 16;
//	Resources.maxFragmentInputVectors = 15;
//	Resources.minProgramTexelOffset = -8;
//	Resources.maxProgramTexelOffset = 7;
//	Resources.maxClipDistances = 8;
//	Resources.maxComputeWorkGroupCountX = 65535;
//	Resources.maxComputeWorkGroupCountY = 65535;
//	Resources.maxComputeWorkGroupCountZ = 65535;
//	Resources.maxComputeWorkGroupSizeX = 1024;
//	Resources.maxComputeWorkGroupSizeY = 1024;
//	Resources.maxComputeWorkGroupSizeZ = 64;
//	Resources.maxComputeUniformComponents = 1024;
//	Resources.maxComputeTextureImageUnits = 16;
//	Resources.maxComputeImageUniforms = 8;
//	Resources.maxComputeAtomicCounters = 8;
//	Resources.maxComputeAtomicCounterBuffers = 1;
//	Resources.maxVaryingComponents = 60;
//	Resources.maxVertexOutputComponents = 64;
//	Resources.maxGeometryInputComponents = 64;
//	Resources.maxGeometryOutputComponents = 128;
//	Resources.maxFragmentInputComponents = 128;
//	Resources.maxImageUnits = 8;
//	Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
//	Resources.maxCombinedShaderOutputResources = 8;
//	Resources.maxImageSamples = 0;
//	Resources.maxVertexImageUniforms = 0;
//	Resources.maxTessControlImageUniforms = 0;
//	Resources.maxTessEvaluationImageUniforms = 0;
//	Resources.maxGeometryImageUniforms = 0;
//	Resources.maxFragmentImageUniforms = 8;
//	Resources.maxCombinedImageUniforms = 8;
//	Resources.maxGeometryTextureImageUnits = 16;
//	Resources.maxGeometryOutputVertices = 256;
//	Resources.maxGeometryTotalOutputComponents = 1024;
//	Resources.maxGeometryUniformComponents = 1024;
//	Resources.maxGeometryVaryingComponents = 64;
//	Resources.maxTessControlInputComponents = 128;
//	Resources.maxTessControlOutputComponents = 128;
//	Resources.maxTessControlTextureImageUnits = 16;
//	Resources.maxTessControlUniformComponents = 1024;
//	Resources.maxTessControlTotalOutputComponents = 4096;
//	Resources.maxTessEvaluationInputComponents = 128;
//	Resources.maxTessEvaluationOutputComponents = 128;
//	Resources.maxTessEvaluationTextureImageUnits = 16;
//	Resources.maxTessEvaluationUniformComponents = 1024;
//	Resources.maxTessPatchComponents = 120;
//	Resources.maxPatchVertices = 32;
//	Resources.maxTessGenLevel = 64;
//	Resources.maxViewports = 16;
//	Resources.maxVertexAtomicCounters = 0;
//	Resources.maxTessControlAtomicCounters = 0;
//	Resources.maxTessEvaluationAtomicCounters = 0;
//	Resources.maxGeometryAtomicCounters = 0;
//	Resources.maxFragmentAtomicCounters = 8;
//	Resources.maxCombinedAtomicCounters = 8;
//	Resources.maxAtomicCounterBindings = 1;
//	Resources.maxVertexAtomicCounterBuffers = 0;
//	Resources.maxTessControlAtomicCounterBuffers = 0;
//	Resources.maxTessEvaluationAtomicCounterBuffers = 0;
//	Resources.maxGeometryAtomicCounterBuffers = 0;
//	Resources.maxFragmentAtomicCounterBuffers = 1;
//	Resources.maxCombinedAtomicCounterBuffers = 1;
//	Resources.maxAtomicCounterBufferSize = 16384;
//	Resources.maxTransformFeedbackBuffers = 4;
//	Resources.maxTransformFeedbackInterleavedComponents = 64;
//	Resources.maxCullDistances = 8;
//	Resources.maxCombinedClipAndCullDistances = 8;
//	Resources.maxSamples = 4;
//	Resources.limits.nonInductiveForLoops = 1;
//	Resources.limits.whileLoops = 1;
//	Resources.limits.doWhileLoops = 1;
//	Resources.limits.generalUniformIndexing = 1;
//	Resources.limits.generalAttributeMatrixVectorIndexing = 1;
//	Resources.limits.generalVaryingIndexing = 1;
//	Resources.limits.generalSamplerIndexing = 1;
//	Resources.limits.generalVariableIndexing = 1;
//	Resources.limits.generalConstantMatrixVectorIndexing = 1;
//}
//
//EShLanguage Shader::FindLanguage(const VkShaderStageFlagBits shader_type) {
//	switch (shader_type) {
//	case VK_SHADER_STAGE_VERTEX_BIT:
//		return EShLangVertex;
//
//	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
//		return EShLangTessControl;
//
//	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
//		return EShLangTessEvaluation;
//
//	case VK_SHADER_STAGE_GEOMETRY_BIT:
//		return EShLangGeometry;
//
//	case VK_SHADER_STAGE_FRAGMENT_BIT:
//		return EShLangFragment;
//
//	case VK_SHADER_STAGE_COMPUTE_BIT:
//		return EShLangCompute;
//
//	default:
//		return EShLangVertex;
//	}
//}

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

