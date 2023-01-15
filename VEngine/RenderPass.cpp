#include "RenderPass.h"

RenderPass::RenderPass(uint32_t renderAreaWidth, uint32_t renderAreaHeight)
{
	renderArea.extent = { renderAreaWidth, renderAreaHeight };
	renderArea.offset = { 0, 0 };

	perFrameUniformBuffer = new UniformBuffer();
}


RenderPass::~RenderPass()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	const VkDescriptorPool descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();

	delete renderBuffer;

	delete perFrameUniformBuffer;

	vkDestroyFramebuffer(logicalDevice, frameBuffer, NULL);
	
	vkDestroyRenderPass(logicalDevice, renderPass, NULL);

	vkFreeDescriptorSets(logicalDevice, descriptorPool, 1, &descriptorSet);

	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, NULL);

	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout[0], NULL);
	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout[1], NULL);

	for (size_t i = 0, count = images.size(); i < count; i++)
	{
		delete images[i];
	}
}

void RenderPass::AddNewSubPass()
{
	//Handle creation of new subpass.

	VkSubpassDescription subpassDescription;
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = NULL;
	subpassDescription.colorAttachmentCount = 0;
	subpassDescription.pColorAttachments = NULL;
	subpassDescription.pResolveAttachments = 0;
	subpassDescription.pDepthStencilAttachment = NULL;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = NULL;
	subpassDescriptions.push_back(subpassDescription);

	subpassReferences.push_back(SubpassReferences());
}

VkAttachmentReference RenderPass::AddColourAttachmentToCurrentSubpass(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat renderbufferFormat)
{
	assert(pixelWidth <= renderArea.extent.width && pixelHeight <= renderArea.extent.height);

	//Image and image views for vulkan side for the things that we are going to actually have to use.
	CreateColourImageAndImageView(pixelWidth, pixelHeight, renderbufferFormat);

	//Description of the colour attachment for the subpass.
	VkAttachmentDescription colourAttachment;
	colourAttachment.format = renderbufferFormat;
	colourAttachment.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colourAttachment.flags = 0;

	//Reference to the attachment for the renderpass
	VkAttachmentReference colourReference;
	colourReference.attachment = (uint32_t)attachmentDescriptions.size(); //The index of the attachment in the pattachments variable in the renderpassCI.
	colourReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Push back all the things!
	attachmentDescriptions.push_back(colourAttachment);
	subpassReferences.back().colourReferences.push_back(colourReference);

	return colourReference;
}

void RenderPass::AddDepthAttachmentToCurrentSubpass(uint32_t pixelWidth, uint32_t pixelHeight)
{
	assert(pixelWidth <= renderArea.extent.width && pixelHeight <= renderArea.extent.height);
	assert(subpassReferences.back().depthReference.attachment == 0);

	//Image and image views for vulkan side for the things that we are going to actually have to use.
	CreateDepthImageAndImageView(pixelWidth, pixelHeight);

	VkAttachmentDescription depthAttachment;
	depthAttachment.format = VkFormat::VK_FORMAT_D32_SFLOAT;
	depthAttachment.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.flags = 0;

	VkAttachmentReference depthReference;
	depthReference.attachment = (uint32_t)attachmentDescriptions.size(); //The index of the attachment in the pattachments variable in the renderpassCI.
	depthReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//The frame buffer descriptions already has this frame buffer in it.
	attachmentDescriptions.push_back(depthAttachment);
	subpassReferences.back().depthReference = depthReference;

	usesDepth = true;
}

void RenderPass::AddInputAttachmentToCurrentSubpass(VkAttachmentReference attachment)
{
	subpassReferences.back().inputReferences.push_back(attachment);
}

void RenderPass::CreateRenderPass()
{
	VkResult result;
	//Create the subpasses
	assert(subpassDescriptions.size() == subpassReferences.size());
	for (size_t i = 0; i < subpassDescriptions.size(); i++)
	{
		subpassDescriptions[i].colorAttachmentCount = (uint32_t)subpassReferences[i].colourReferences.size();
		subpassDescriptions[i].pColorAttachments = subpassReferences[i].colourReferences.data();
		if (subpassReferences[i].depthReference.attachment != 0)
		{
			subpassDescriptions[i].pDepthStencilAttachment = &subpassReferences[i].depthReference;
		}
	}

	//Create the render pass
	VkRenderPassCreateInfo renderPassCI;
	renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCI.flags = 0;
	renderPassCI.pNext = NULL;
	renderPassCI.attachmentCount = (uint32_t)attachmentDescriptions.size();
	renderPassCI.pAttachments = attachmentDescriptions.data();
	renderPassCI.subpassCount = (uint32_t)subpassDescriptions.size();
	renderPassCI.pSubpasses = subpassDescriptions.data();
	renderPassCI.dependencyCount = 0;
	renderPassCI.pDependencies = NULL;

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	result = vkCreateRenderPass(logicalDevice, &renderPassCI, NULL, &renderPass);
	assert(result == VK_SUCCESS);


	//Fetch the image views for the framebuffer creation
	vkImageViews.clear();
	for (size_t i = 0, count = images.size(); i < count; i++)
	{
		vkImageViews.push_back(images[i]->GetImageView());
	}

	//frame buffer creation
	VkFramebufferCreateInfo frameBufferCI;
	frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCI.pNext = NULL;
	frameBufferCI.flags = 0;
	frameBufferCI.renderPass = renderPass;
	frameBufferCI.attachmentCount = (uint32_t)vkImageViews.size();
	frameBufferCI.pAttachments = vkImageViews.data();
	frameBufferCI.width = renderArea.extent.width;
	frameBufferCI.height = renderArea.extent.height;
	frameBufferCI.layers = 1;

	result = vkCreateFramebuffer(logicalDevice, &frameBufferCI, NULL, &frameBuffer);
	assert(result == VK_SUCCESS);

	//fetch the clear colours for the render pass begin info
	clearValues.clear();
	for (size_t i = 0, count = images.size(); i < count; i++)
	{
		clearValues.push_back(images[i]->GetClearValue());
	}

	//render pass begin info
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = 0;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea = renderArea;
	renderPassBeginInfo.framebuffer = frameBuffer;
	renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
	renderPassBeginInfo.pClearValues = clearValues.data();
	
	//Allocate command buffer
	renderBuffer = new CommandBuffer(CommandBufferType::Graphics, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	renderBuffer->SetDestinationStageMask(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	// Descriptor Set Bindings //
	std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
	std::vector<VkDescriptorSetLayoutBinding> perDrawDescriptorBindings;

	//Per-Frame Data
	descriptorBindings.push_back
	(
	{	//VkDescriptorSetLayoutBinding
		0,											//binding;				//Binding within the descriptor set
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			//descriptorType;		
		1,											//descriptorCount;		//For use with an array of objects
		VkShaderStageFlagBits::VK_SHADER_STAGE_ALL,	//stageFlags;			//Stages of the shader that CAN access the descriptor
		NULL										//pImmutableSamplers;
	}
	);

	//Per-Pass Data
	descriptorBindings.push_back
	(
	{	//VkDescriptorSetLayoutBinding
		1,											//binding;				
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			//descriptorType;		
		1,											//descriptorCount;		
		VkShaderStageFlagBits::VK_SHADER_STAGE_ALL,	//stageFlags;			
		NULL										//pImmutableSamplers;
	}
	);

	//Per-Draw Data
	perDrawDescriptorBindings.push_back
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
	perDrawDescriptorBindings.push_back
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
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI[2];
	descriptorSetLayoutCI[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCI[0].pNext = NULL;
	descriptorSetLayoutCI[0].flags = 0;
	descriptorSetLayoutCI[0].bindingCount = (uint32_t)descriptorBindings.size();
	descriptorSetLayoutCI[0].pBindings = descriptorBindings.data();

	descriptorSetLayoutCI[1].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCI[1].pNext = NULL;
	descriptorSetLayoutCI[1].flags = 0;
	descriptorSetLayoutCI[1].bindingCount = (uint32_t)perDrawDescriptorBindings.size();
	descriptorSetLayoutCI[1].pBindings = perDrawDescriptorBindings.data();

	result = vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCI[0], NULL, &descriptorSetLayout[0]);
	result = vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCI[1], NULL, &descriptorSetLayout[1]);
	assert(result == VK_SUCCESS);

	//Allocate descriptor set
	VkDescriptorSetAllocateInfo descirptorSetAlloc;
	descirptorSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descirptorSetAlloc.pNext = NULL;
	descirptorSetAlloc.descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();
	descirptorSetAlloc.descriptorSetCount = 1;
	descirptorSetAlloc.pSetLayouts = &descriptorSetLayout[0];

	result = vkAllocateDescriptorSets(logicalDevice, &descirptorSetAlloc, &descriptorSet);
	assert(result == VK_SUCCESS);

	//Update the descriptor set with nothing to make vulklan stop complaining;
	vkUpdateDescriptorSets(logicalDevice, 0, NULL, 0, NULL);

	//Push Constant Ranges
	VkPushConstantRange pushConstantRange;
	pushConstantRange.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
	pushConstantRange.offset = 0;
	pushConstantRange.size = 128; //Minimum guaranteed size of a push constant. Probably should use more than this anyway.

	// Pipeline Layout //
	VkPipelineLayoutCreateInfo pipelineLayoutCI;
	pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCI.pNext = NULL;
	pipelineLayoutCI.flags = 0;
	pipelineLayoutCI.setLayoutCount = 2;
	pipelineLayoutCI.pSetLayouts = descriptorSetLayout;
	pipelineLayoutCI.pushConstantRangeCount = 1;
	pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

	result = vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCI, NULL, &pipelineLayout);
	assert(result == VK_SUCCESS);
}

void RenderPass::BeginPass(glm::mat4 &vMatrix, glm::mat4 &pMatrix, glm::mat4 &vpMatrix)
{
	//begin recording buffer;
	renderBuffer->BeginRecording();
	VkCommandBuffer vkRenderBuffer = renderBuffer->GetVKCommandBuffer();

	//bind viewport and scissor
	viewport.height = (float)renderArea.extent.height;
	viewport.width = (float)renderArea.extent.width;
	viewport.minDepth = (float)0.0f;
	viewport.maxDepth = (float)1.0f;
	viewport.x = 0;
	viewport.y = 0;

	vkCmdSetViewport(vkRenderBuffer, 0, 1, &viewport);
	vkCmdSetScissor(vkRenderBuffer, 0, 1, &renderArea);

	//Clear the images.
	for (size_t i = 0, count = images.size(); i < count; i++)
	{
		images[i]->CmdClearImage(*renderBuffer);
	}

	//Render pass
	vkCmdBeginRenderPass(vkRenderBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

	size_t mat4Size = sizeof(glm::mat4);

	//Update the PerFrameUniformBuffer
	perFrameUniformBuffer->SetBufferData((void*)&vMatrix, mat4Size, 0);
	perFrameUniformBuffer->SetBufferData((void*)&pMatrix, mat4Size, (int)mat4Size);
	perFrameUniformBuffer->SetBufferData((void*)&vpMatrix, mat4Size, (int)(mat4Size + mat4Size));

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	VkDescriptorBufferInfo perFrameBufferInfo;
	perFrameBufferInfo.buffer = perFrameUniformBuffer->GetVKBuffer();
	perFrameBufferInfo.offset = 0;
	perFrameBufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = NULL;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.pImageInfo = NULL;
	descriptorWrite.pBufferInfo = &perFrameBufferInfo;
	descriptorWrite.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, NULL);

	vkCmdBindDescriptorSets(vkRenderBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
}

void RenderPass::DrawObjects(std::vector<GameObject*>* objectList, glm::mat4 &vMatrix, glm::mat4 &vpMatrix)
{
	int renderPassIndex = 0;
	Material* objectMaterial = nullptr;
	for (int renderPassIndex = 0, numPasses = (int)subpassDescriptions.size(); renderPassIndex < numPasses; renderPassIndex++)
	{
		// Render pass contents //
		for (size_t i = 0, count = (*objectList).size(); i < count; i++)
		{
			objectMaterial = (*objectList)[i]->GetMaterial();
			objectMaterial->BindMaterial((*objectList)[i]->GetTransform()->GetModelMat(), vMatrix, vpMatrix, renderBuffer, pipelineLayout);

			//Draw the model in the buffer.
			(*objectList)[i]->GetRenderer()->Draw(renderBuffer->GetVKCommandBuffer());
		}

		if (renderPassIndex < numPasses - 1)
			vkCmdNextSubpass(renderBuffer->GetVKCommandBuffer(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
	}
}

void RenderPass::EndPass()
{
	//end render pass
	vkCmdEndRenderPass(renderBuffer->GetVKCommandBuffer());

	//finish recording buffer
	renderBuffer->EndRecording();
}

void RenderPass::RecordBuffer(glm::mat4 &vMatrix, glm::mat4 &pMatrix, glm::mat4 &vpMatrix, std::vector<GameObject*>* objectList)
{
	BeginPass(vMatrix, pMatrix, vpMatrix);
	
	DrawObjects(objectList, vMatrix, vpMatrix);

	EndPass();
}

void RenderPass::SubmitBuffer()
{
	renderBuffer->SubmitBuffer();
}

void RenderPass::ResetBuffer()
{
	renderBuffer->ResetBuffer();
}

void RenderPass::CreateColourImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat imageFormat)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	images.push_back(new Image(pixelWidth, pixelHeight, imageFormat, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT));
	images.back()->ChangeImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void RenderPass::CreateDepthImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	images.push_back(new Image(pixelWidth, pixelHeight, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT));
	images.back()->ChangeImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

Image* RenderPass::GetImage(unsigned int index)
{
	assert(images.size() > 0);
	assert(images.size() - 1 >= index);
	
	return images[index];
}