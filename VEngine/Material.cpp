#include "Material.h"


Material::Material()
{
	int sizeOfVertex = sizeof(glm::vec3), sizeOfNormals = sizeof(glm::vec3), sizeOfUVs = sizeof(glm::vec2);
	//Description of binding and attributes for pipeline
 	viBinding.binding = 0; //Index of the array
 	viBinding.stride = sizeOfVertex + sizeOfNormals + sizeOfUVs; //Number of bytes from one vertex data set to the next
 	viBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //As apposed to instance rendering.
 
 
    //Vertices
 	viAttribs.push_back
 	(
 		{									//VkVertexInputBindingDescription
 			0,								//location	//Location value in vertex shader. Because we need that.
 			0,								//binding	//Which binding the per-vertex data comes from.
 			VK_FORMAT_R32G32B32_SFLOAT,		//format	//Format of the data. float3
 			0								//offset	//Number of bytes from the start of the data to begin.
 		}
 	);		

	//Normals
	viAttribs.push_back
	(
		{
			1,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			(uint32_t)sizeOfVertex
		}
	);
 
 	//UVs
 	viAttribs.push_back
 	(
 		{								
 			2,							
 			0,							
 			VK_FORMAT_R32G32_SFLOAT,	
			(uint32_t)(sizeOfVertex + sizeOfNormals)
 		}
 	);			
}

Material::~Material()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	const VkDescriptorPool descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();

	vkDestroyPipelineLayout(logicalDevice, pipelineData.pipelineLayout, NULL);

	vkDestroyPipeline(logicalDevice, pipelineData.pipeline, NULL);

	for (size_t i = 0, countI = uniformBuffers.size(); i < countI; i++)
	{
		for (size_t k = 0, countK = uniformBuffers[i].size(); i < countK; i++)
		{
			delete uniformBuffers[i][k];
		}
	}

	vkFreeDescriptorSets(logicalDevice, descriptorPool, uniformSets.size(), uniformSets.data());
}

void Material::AddShader(Shader &newShader)
{
	VkPipelineShaderStageCreateInfo shaderStage;
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.pNext = NULL;
	shaderStage.flags = 0;
	shaderStage.stage = (VkShaderStageFlagBits)newShader.GetVKShaderStage();
	shaderStage.module = newShader.GetVKShaderModule();
	shaderStage.pName = "main";
	shaderStage.pSpecializationInfo = NULL;

	shaderStages.push_back(shaderStage);

	layoutDescriptors.push_back(newShader.GetVKDescriptorSetLayout());

	// Uniform Buffer Creation //
	std::vector<size_t> uniformBufferSizes = newShader.GetUniformBytes();

	uniformBuffers.push_back(std::vector<GPUBuffer*>());
	uniformBuffers.back().reserve(uniformBufferSizes.size());

	for (size_t i = 0, count = uniformBufferSizes.size(); i < count; i++)
	{
		uniformBuffers.back().push_back(new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniformBufferSizes[i]));
	}
}

void Material::FinalizeMaterial(VkRenderPass renderPass)
{
	VkResult res;
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	// Pipeline Layout //
	VkPushConstantRange pushConstantRange;
	pushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
	pushConstantRange.offset = 0;
	pushConstantRange.size = 128; //Guaranteed minimum size for the push constant range for a single pipeline layout.

	VkPipelineLayoutCreateInfo pipelineLayoutCI;
	pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCI.pNext = NULL;
	pipelineLayoutCI.flags = 0;
	pipelineLayoutCI.setLayoutCount = layoutDescriptors.size();
	pipelineLayoutCI.pSetLayouts = layoutDescriptors.data();
	pipelineLayoutCI.pushConstantRangeCount = 1;
	pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

	res = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCI, NULL, &pipelineData.pipelineLayout);
	assert(res == VK_SUCCESS);

	// Graphics Pipeline Description //
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pNext = NULL;
	dynamicState.pDynamicStates = dynamicStateEnables;
	dynamicState.dynamicStateCount = 0;

	VkPipelineVertexInputStateCreateInfo vi;
	memset(&vi, 0, sizeof(vi));
	vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.pNext = NULL;
	vi.flags = 0;
	vi.vertexBindingDescriptionCount = 1;
	vi.pVertexBindingDescriptions = &viBinding;
	vi.vertexAttributeDescriptionCount = viAttribs.size();
	vi.pVertexAttributeDescriptions = viAttribs.data();

	VkPipelineInputAssemblyStateCreateInfo ia;
	ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.pNext = NULL;
	ia.flags = 0;
	ia.primitiveRestartEnable = VK_FALSE;
	ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineRasterizationStateCreateInfo rs;
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext = NULL;
	rs.flags = 0;
	rs.polygonMode = VK_POLYGON_MODE_FILL;
	rs.cullMode = VK_CULL_MODE_BACK_BIT;
	rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rs.depthClampEnable = VK_FALSE;
	rs.rasterizerDiscardEnable = VK_FALSE;
	rs.depthBiasEnable = VK_FALSE;
	rs.depthBiasConstantFactor = 0;
	rs.depthBiasClamp = 0;
	rs.depthBiasSlopeFactor = 0;
	rs.lineWidth = 1.0f;

	VkPipelineColorBlendStateCreateInfo cb;
	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.pNext = NULL;
	cb.flags = 0;
	VkPipelineColorBlendAttachmentState att_state[1];
	att_state[0].colorWriteMask = 0xf;
	att_state[0].blendEnable = VK_FALSE;
	att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
	att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
	att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	cb.attachmentCount = 1;
	cb.pAttachments = att_state;
	cb.logicOpEnable = VK_FALSE;
	cb.logicOp = VK_LOGIC_OP_NO_OP;
	cb.blendConstants[0] = 1.0f;
	cb.blendConstants[1] = 1.0f;
	cb.blendConstants[2] = 1.0f;
	cb.blendConstants[3] = 1.0f;

	VkPipelineViewportStateCreateInfo vp = {};
	vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.pNext = NULL;
	vp.flags = 0;
	vp.viewportCount = NUM_VIEWPORTS;
	dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
	vp.scissorCount = NUM_SCISSORS;
	dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
	vp.pScissors = NULL;
	vp.pViewports = NULL;

	VkPipelineDepthStencilStateCreateInfo ds;
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.pNext = NULL;
	ds.flags = 0;
	ds.depthTestEnable = VK_TRUE;
	ds.depthWriteEnable = VK_TRUE;
	ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	ds.depthBoundsTestEnable = VK_FALSE;
	ds.minDepthBounds = 0;
	ds.maxDepthBounds = 0;
	ds.stencilTestEnable = VK_FALSE;
	ds.back.failOp = VK_STENCIL_OP_KEEP;
	ds.back.passOp = VK_STENCIL_OP_KEEP;
	ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
	ds.back.compareMask = 0;
	ds.back.reference = 0;
	ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
	ds.back.writeMask = 0;
	ds.front = ds.back;

	VkPipelineMultisampleStateCreateInfo ms;
	ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.pNext = NULL;
	ms.flags = 0;
	ms.pSampleMask = NULL;
	ms.rasterizationSamples = NUM_SAMPLES;
	ms.sampleShadingEnable = VK_FALSE;
	ms.alphaToCoverageEnable = VK_FALSE;
	ms.alphaToOneEnable = VK_FALSE;
	ms.minSampleShading = 0.0;

	VkGraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = NULL;
	pipelineInfo.layout = pipelineData.pipelineLayout;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = 0;
	pipelineInfo.flags = 0;
	pipelineInfo.pVertexInputState = &vi;
	pipelineInfo.pInputAssemblyState = &ia;
	pipelineInfo.pRasterizationState = &rs;
	pipelineInfo.pColorBlendState = &cb;
	pipelineInfo.pTessellationState = NULL;
	pipelineInfo.pMultisampleState = &ms;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.pViewportState = &vp;
	pipelineInfo.pDepthStencilState = &ds;
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	res = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipelineData.pipeline);
	assert(res == VK_SUCCESS);


	// Uniform allocations //
	uniformSets.resize(layoutDescriptors.size());

	VkDescriptorSetAllocateInfo descriptorSetCI;
	descriptorSetCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetCI.pNext = NULL;
	descriptorSetCI.descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();
	descriptorSetCI.descriptorSetCount = layoutDescriptors.size();
	descriptorSetCI.pSetLayouts = layoutDescriptors.data();

	vkAllocateDescriptorSets(logicalDevice, &descriptorSetCI, uniformSets.data());
}

void Material::BindPipeline(CommandBuffer &commandBuffer)
{
	const VkCommandBuffer buffer = commandBuffer.GetVKCommandBuffer();
	vkCmdBindPipeline(buffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipeline);

	vkCmdBindDescriptorSets(buffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipelineLayout, 0, uniformSets.size(), uniformSets.data(), 0, NULL);
}

void Material::SetUniform_Mat4x4(glm::mat4x4 &data, int uniformSet, int binding)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	uint64_t bufferSize = sizeof(data);

	//Place the data into the uniform buffer
	GPUBuffer* uniformBuffer = uniformBuffers[uniformSet][binding];

	uniformBuffer->CopyMemoryIntoBuffer(&data, bufferSize);

	//Buffer info that describes how to use the buffer to update the uniform
	VkDescriptorBufferInfo uniformBufferInfo;
	uniformBufferInfo.buffer = uniformBuffer->GetVKBuffer();
	uniformBufferInfo.offset = 0;
	uniformBufferInfo.range = bufferSize;

	//Descriptor write that tells Vulkan what we're updating and what we're updating it with
	uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uniformWrite.pNext = NULL;
	uniformWrite.dstSet = uniformSets[uniformSet];
	uniformWrite.dstBinding = binding;
	uniformWrite.dstArrayElement = 0;
	uniformWrite.descriptorCount = 1;
	uniformWrite.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformWrite.pImageInfo = NULL;
	uniformWrite.pBufferInfo = &uniformBufferInfo;
	uniformWrite.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(logicalDevice, 1, &uniformWrite, 0, NULL);
}

void Material::SetTexture(Texture& texture, int uniformSet, int binding)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//Image infor the describes the image to 
	VkDescriptorImageInfo imageInfo;
	imageInfo.sampler = texture.GetSampler();
	imageInfo.imageView = texture.GetImageView();
	imageInfo.imageLayout = texture.GetImageLayout();

	//Descriptor write that tells Vulkan what we're updating and what we're updating it with
	uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uniformWrite.pNext = NULL;
	uniformWrite.dstSet = uniformSets[uniformSet];
	uniformWrite.dstBinding = binding;
	uniformWrite.dstArrayElement = 0;
	uniformWrite.descriptorCount = 1;
	uniformWrite.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	uniformWrite.pImageInfo = &imageInfo;
	uniformWrite.pBufferInfo = NULL;
	uniformWrite.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(logicalDevice, 1, &uniformWrite, 0, NULL);
}