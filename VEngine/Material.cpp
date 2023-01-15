#include "Material.h"


Material::Material()
{
	int sizeOfVertex = sizeof(glm::vec3), sizeOfNormals = sizeof(glm::vec3), sizeOfUVs = sizeof(glm::vec2), sizeOfTangent = sizeof(glm::vec3), sizeOfBitangent = sizeof(glm::vec3);
	//Description of binding and attributes for pipeline
	viBindings.push_back(VkVertexInputBindingDescription()); //PerVertexData
 	viBindings[0].binding = 0; //Index of the array
 	viBindings[0].stride = sizeOfVertex + sizeOfNormals + sizeOfUVs + sizeOfTangent + sizeOfBitangent; //Number of bytes from one vertex data set to the next
 	viBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //As apposed to instance rendering.

	viBindings.push_back(VkVertexInputBindingDescription()); //PerInstanceData
	viBindings[1].binding = 1; //Index of the array
	viBindings[1].stride = sizeof(glm::vec3) + sizeof(glm::quat); //Number of bytes from one vertex data set to the next
	viBindings[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE; //As apposed to instance rendering.
 
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

	//Tangents
	viAttribs.push_back
	(
		{
			3,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			(uint32_t)(sizeOfVertex + sizeOfNormals + sizeOfUVs)
		}
	);

	//Bitangents
	viAttribs.push_back
	(
		{
			4,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			(uint32_t)(sizeOfVertex + sizeOfNormals + sizeOfUVs + sizeOfTangent)
		}
	);

	// local position
	viAttribs.push_back
	(
		{
			5,
			1,
			VK_FORMAT_R32G32B32_SFLOAT,
			0
		}
	);

	// local rotation
	viAttribs.push_back
	(
		{
			6,
			1,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			(uint32_t)sizeOfVertex
		}
	);


	// Descriptor Sets //

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	// Uniforms 
	uniformBuffer = new UniformBuffer();

	//Per-Draw Data
	descriptorBindings.push_back
	(
		{	//VkDescriptorSetLayoutBinding
			0,											//binding;				
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			//descriptorType;		
			1,											//descriptorCount;		
			VkShaderStageFlagBits::VK_SHADER_STAGE_ALL,	//stageFlags;			
			NULL										//pImmutableSamplers;
		}
	);

	//Passed Textures
	descriptorBindings.push_back
	(
		{	//VkDescriptorSetLayoutBinding
			1,											//binding;				
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	//descriptorType;		
			6,											//descriptorCount;		
			VkShaderStageFlagBits::VK_SHADER_STAGE_ALL,	//stageFlags;			
			NULL										//pImmutableSamplers;
		}
	);

	// Descriptor Set Layout //
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI;
	descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCI.pNext = NULL;
	descriptorSetLayoutCI.flags = 0;
	descriptorSetLayoutCI.bindingCount = (uint32_t)descriptorBindings.size();
	descriptorSetLayoutCI.pBindings = descriptorBindings.data();

	vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCI, NULL, &descriptorSetLayout);

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = NULL;
	descriptorSetAllocateInfo.descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &uniformDescriptorSet);

	//Textures
	textures.reserve(5);

	//Instance buffer.
	//This solution is a temporary one. I'd like to organize this better.
	instanceBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(glm::vec3) + sizeof(glm::quat));
}

Material::~Material()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	vkDestroyPipeline(logicalDevice, pipelineData.pipeline, NULL);
	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, NULL);

	delete uniformBuffer;
	delete instanceBuffer;
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
}

void Material::FinalizeMaterial(RenderPass &renderPass, VkPrimitiveTopology primitiveType)
{
	VkResult res;
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

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
	vi.vertexBindingDescriptionCount = (uint32_t)viBindings.size();
	vi.pVertexBindingDescriptions = viBindings.data();
	vi.vertexAttributeDescriptionCount = (uint32_t)viAttribs.size();
	vi.pVertexAttributeDescriptions = viAttribs.data();

	VkPipelineInputAssemblyStateCreateInfo ia;
	ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.pNext = NULL;
	ia.flags = 0;
	ia.primitiveRestartEnable = VK_FALSE;
	ia.topology = primitiveType;

	VkPipelineRasterizationStateCreateInfo rs;
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext = NULL;
	rs.flags = 0;
	rs.polygonMode = VK_POLYGON_MODE_FILL;
	rs.cullMode = VK_CULL_MODE_NONE;// VK_CULL_MODE_BACK_BIT;
	rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rs.depthClampEnable = VK_FALSE;
	rs.rasterizerDiscardEnable = VK_FALSE;
	rs.depthBiasEnable = VK_FALSE;
	rs.depthBiasConstantFactor = 0;
	rs.depthBiasClamp = 0;
	rs.depthBiasSlopeFactor = 0;
	rs.lineWidth = 60.0f;

	VkPipelineColorBlendStateCreateInfo cb;
	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.pNext = NULL;
	cb.flags = 0;
	std::vector<VkPipelineColorBlendAttachmentState> att_state;
	att_state.resize(renderPass.GetAttachmentsCount());
	for (size_t i = 0, length = att_state.size(); i < length; i++)
	{
		att_state[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;;
		att_state[i].blendEnable = VK_TRUE;
		att_state[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		att_state[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		att_state[i].colorBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
		att_state[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		att_state[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		att_state[i].alphaBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
	}
	cb.attachmentCount = (uint32_t)renderPass.GetAttachmentsCount();
	cb.pAttachments = att_state.data();
	cb.logicOpEnable = VK_FALSE;
	cb.logicOp = VK_LOGIC_OP_NO_OP;
	cb.blendConstants[0] = 0.0f;
	cb.blendConstants[1] = 0.0f;
	cb.blendConstants[2] = 0.0f;
	cb.blendConstants[3] = 0.0f;

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
	ds.depthCompareOp = VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
	ds.depthBoundsTestEnable = VK_FALSE;
	ds.minDepthBounds = 0;
	ds.maxDepthBounds = 1;
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
	pipelineInfo.layout = pipelineData.layout = renderPass.GetVKPipelineLayout();
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
	pipelineInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineInfo.renderPass = renderPass.GetVKRenderPass();
	pipelineInfo.subpass = 0;

	res = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipelineData.pipeline);
	assert(res == VK_SUCCESS);
}

void Material::BindMaterial(glm::mat4 &mMatrix, glm::mat4 &vMatrix, glm::mat4 &vpMatrix, CommandBuffer *renderBuffer, VkPipelineLayout pipelineLayout)
{
	SetDrawMatrices(mMatrix, vMatrix, vpMatrix);

	UpdateDescriptorSet();

	//Bind the material?
	BindPipeline(*renderBuffer);

	//Bind uniforms
	BindPerDrawUniforms(renderBuffer->GetVKCommandBuffer(), pipelineLayout);
}

void Material::BindPipeline(CommandBuffer &commandBuffer)
{
	vkCmdBindPipeline(commandBuffer.GetVKCommandBuffer(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipeline);
}

void Material::UpdateDescriptorSet()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = uniformBuffer->GetVKBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = NULL;
	descriptorWrite.dstSet = uniformDescriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.pImageInfo = NULL;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, NULL);

	for (size_t i = 0, count = textures.size(); i < count; i++)
	{
		//Descriptor write that tells Vulkan what we're updating and what we're updating it with
		uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		uniformWrite.pNext = NULL;
		uniformWrite.dstSet = uniformDescriptorSet;
		uniformWrite.dstBinding = PerDrawUniformTextureBinding;
		uniformWrite.dstArrayElement = (uint32_t)i;
		uniformWrite.descriptorCount = 1;
		uniformWrite.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		uniformWrite.pImageInfo = &textures[i];
		uniformWrite.pBufferInfo = NULL;
		uniformWrite.pTexelBufferView = NULL;

		vkUpdateDescriptorSets(logicalDevice, 1, &uniformWrite, 0, NULL);
	}
}

void Material::BindPerDrawUniforms(VkCommandBuffer vkRenderBuffer, VkPipelineLayout pipelineLayout)
{
	vkCmdBindDescriptorSets(vkRenderBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &uniformDescriptorSet, 0, NULL);
}

void Material::SetDrawMatrices(glm::mat4 & modelMat, glm::mat4 & viewMat, glm::mat4 & viewProjoectionMat)
{
	size_t mat4Size = sizeof(glm::mat4);

	//Model Matrix
	SetUniform_Mat4x4(modelMat, 0);

	//Model View Matrix
	SetUniform_Mat4x4(viewMat * modelMat, (int)mat4Size);

	//Model View Projection Matrix
	SetUniform_Mat4x4(viewProjoectionMat * modelMat, (int)(mat4Size + mat4Size));

	instanceData.position = { modelMat[3].x, modelMat[3].y, modelMat[3].z };
	instanceData.rotation = glm::toQuat(modelMat);
	instanceBuffer->CopyMemoryIntoBuffer(&instanceData, sizeof(InstanceData));
}

void Material::SetUniform_Mat4x4(glm::mat4x4 &data, int offset)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//Place the data into the uniform buffer
	uniformBuffer->SetBufferData(&data, sizeof(data), offset);
}

void Material::SetUniform_Int32(int &data, int offset)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//Place the data into the uniform buffer
	uniformBuffer->SetBufferData(&data, sizeof(data), offset);
}

void Material::SetUniform_Float32(float &data, int offset)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//Place the data into the uniform buffer
	uniformBuffer->SetBufferData(&data, sizeof(data), offset);
}

void Material::SetTexture(Texture& texture, int offset)
{
	textures.push_back(VkDescriptorImageInfo());

	textures.back().sampler = texture.GetSampler();
	textures.back().imageView = texture.GetImageView();
	textures.back().imageLayout = texture.GetImageLayout();
}

void Material::SetTexture(RenderTexture& texture, int offset)
{
	textures.push_back(VkDescriptorImageInfo());

	textures.back().sampler = texture.GetSampler();
	textures.back().imageView = texture.GetImageView();
	textures.back().imageLayout = texture.GetImageLayout();
}

