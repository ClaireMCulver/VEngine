#include "RenderPass.h"

RenderPass::RenderPass()
{
	renderArea.extent = { 0, 0 };
	renderArea.offset = { 0, 0 };
}


RenderPass::~RenderPass()
{
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

void RenderPass::AddColourAttachementToCurrentSubpass(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat renderbufferFormat, bool useDepthBuffer)
{
	if (pixelWidth > renderArea.extent.width)
	{
		renderArea.extent.width = pixelWidth;
	}
	if (pixelHeight > renderArea.extent.height)
	{
		renderArea.extent.height = pixelHeight;
	}

	//Image and image views for vulkan side for the things that we are going to actually have to use.
	CreateImageAndImageView(pixelWidth, pixelHeight, renderbufferFormat, useDepthBuffer);

	//Description of the colour attachment for the subpass.
	VkAttachmentDescription colourAttachment;
	colourAttachment.format = renderbufferFormat;
	colourAttachment.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	colourAttachment.flags = 0;

	//Reference to the attachment for the renderpass
	VkAttachmentReference colourReference;
	colourReference.attachment = attachmentDescriptions.size(); //The index of the attachment in the pattachments variable in the renderpassCI.
	colourReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Push back all the things!
	attachmentDescriptions.push_back(colourAttachment);
	subpassReferences.back().colourReferences.push_back(colourReference);

	if (useDepthBuffer)
	{
		VkAttachmentDescription depthAttachment;
		depthAttachment.format = VkFormat::VK_FORMAT_D16_UNORM;
		depthAttachment.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		depthAttachment.flags = 0;

		VkAttachmentReference depthReference;
		depthReference.attachment = attachmentDescriptions.size(); //The index of the attachment in the pattachments variable in the renderpassCI.
		depthReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//The frame buffer descriptions already has this frame buffer in it.
		attachmentDescriptions.push_back(depthAttachment);
		subpassReferences.back().depthReference = depthReference;
	}
}

void RenderPass::CreateRenderPass()
{
	VkResult result;
	//Create the subpasses
	assert(subpassDescriptions.size() == subpassReferences.size());
	for (size_t i = 0; i < subpassDescriptions.size(); i++)
	{
		subpassDescriptions[i].colorAttachmentCount = subpassReferences[i].colourReferences.size();
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
	renderPassCI.attachmentCount = attachmentDescriptions.size();
	renderPassCI.pAttachments = attachmentDescriptions.data();
	renderPassCI.subpassCount = subpassDescriptions.size();
	renderPassCI.pSubpasses = subpassDescriptions.data();
	renderPassCI.dependencyCount = 0;
	renderPassCI.pDependencies = NULL;

	const VkDevice logicalDevice = GraphicsSystem::GetLogicalDevice();

	result = vkCreateRenderPass(logicalDevice, &renderPassCI, NULL, &renderPass);
	assert(result == VK_SUCCESS);



	//frame buffer creation
	VkFramebufferCreateInfo frameBufferCI;
	frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCI.pNext = NULL;
	frameBufferCI.flags = 0;
	frameBufferCI.renderPass = renderPass;
	frameBufferCI.attachmentCount = imageViews.size();
	frameBufferCI.pAttachments = imageViews.data();
	frameBufferCI.width = renderArea.extent.width;
	frameBufferCI.height = renderArea.extent.height;
	frameBufferCI.layers = 1;

	result = vkCreateFramebuffer(logicalDevice, &frameBufferCI, NULL, &frameBuffer);
	assert(result == VK_SUCCESS);

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = 0;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea = renderArea;
	renderPassBeginInfo.framebuffer = frameBuffer;
	renderPassBeginInfo.clearValueCount = clearValues.size();
	renderPassBeginInfo.pClearValues = clearValues.data();

	//Create semaphore for completion of the renderpass
	VkSemaphoreCreateInfo semaphoreCI;
	semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCI.pNext = NULL;
	semaphoreCI.flags = 0;

	result = vkCreateSemaphore(logicalDevice, &semaphoreCI, NULL, &renderFinishedSemaphore);
	assert(result == VK_SUCCESS);

	
	//Allocate command buffer
	VkCommandBufferAllocateInfo cmdCI;
	cmdCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdCI.pNext = NULL;
	cmdCI.commandPool = GraphicsSystem::GetSingleton()->GetCommandPool();
	cmdCI.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	
	result = vkAllocateCommandBuffers(logicalDevice, &cmdCI, &renderBuffer);
	assert(result == VK_SUCCESS);
}

void RenderPass::BindRenderPass(VkCommandBuffer &cmdBuffer)
{
	vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::UnbindRenderPass(VkCommandBuffer &cmdBuffer)
{
	vkCmdEndRenderPass(cmdBuffer);
}

void RenderPass::RecordBuffer()
{
	VkCommandBufferBeginInfo cmdBI;
	cmdBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBI.pNext = NULL;
	cmdBI.flags = 0;
	cmdBI.pInheritanceInfo = NULL;

	//begin recording buffer;
	vkBeginCommandBuffer(renderBuffer, &cmdBI);

	//bind viewport and scissor
	viewport.height = (float)renderArea.extent.height;
	viewport.width = (float)renderArea.extent.width;
	viewport.minDepth = (float)0.0f;
	viewport.maxDepth = (float)1.0f;
	viewport.x = 0;
	viewport.y = 0;

	vkCmdSetViewport(renderBuffer, 0, 1, &viewport);
	vkCmdSetScissor(renderBuffer, 0, 1, &renderArea);

	//Render pass
	vkCmdBeginRenderPass(renderBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

	// Render pass contents //
		for (size_t i = 0, count = registeredMeshes.size(); i < count; i++)
		{
			registeredMeshes[i]->Render(renderBuffer);
		}

	//end render pass
	vkCmdEndRenderPass(renderBuffer);

	//finish recording buffer
	vkEndCommandBuffer(renderBuffer);

	bufferSubmitInfo.pNext = NULL;
	bufferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	bufferSubmitInfo.waitSemaphoreCount = 0;
	bufferSubmitInfo.pWaitSemaphores = NULL;
	bufferSubmitInfo.pWaitDstStageMask = &pipelineStage;
	bufferSubmitInfo.commandBufferCount = 1;
	bufferSubmitInfo.pCommandBuffers = &renderBuffer;
	bufferSubmitInfo.signalSemaphoreCount = 1;
	bufferSubmitInfo.pSignalSemaphores = &renderFinishedSemaphore;
}

void RenderPass::SubmitBuffer()
{
	vkQueueSubmit(GraphicsSystem::GetSingleton()->GetGraphicsQueue(), 1, &bufferSubmitInfo, NULL);
}

void RenderPass::CreateImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat imageFormat, bool hasDepthBuffer)
{
	const VkDevice logicalDevice = GraphicsSystem::GetLogicalDevice();

	// Colour attachement creation //

	//image creation
	VkImageCreateInfo colourImageCI;
	colourImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	colourImageCI.pNext = NULL;
	colourImageCI.flags = 0; //Additional flags are in VkImageCreateFlagBits
	colourImageCI.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	colourImageCI.format = imageFormat;
	colourImageCI.extent = VkExtent3D{ pixelWidth, pixelHeight, 1 };
	colourImageCI.mipLevels = 0;
	colourImageCI.arrayLayers = 1;
	colourImageCI.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	colourImageCI.tiling = VkImageTiling::VK_IMAGE_TILING_LINEAR; //TODO: dig into what these mean.
	colourImageCI.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	colourImageCI.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
	colourImageCI.queueFamilyIndexCount = 0;
	colourImageCI.pQueueFamilyIndices = NULL; //Ignorable because sharingMode is not concurrent.
	colourImageCI.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	images.push_back(VkImage());
	vkCreateImage(logicalDevice, &colourImageCI, NULL, &images.back());

	VkClearValue clearColourValue;
	clearColourValue.color.float32[0] = 0.2f;
	clearColourValue.color.float32[1] = 0.2f;
	clearColourValue.color.float32[2] = 0.2f;
	clearColourValue.color.float32[3] = 0.2f;
	clearValues.push_back(clearColourValue);

	//image view creation
	VkImageViewCreateInfo colourViewCI;
	colourViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	colourViewCI.pNext = NULL;
	colourViewCI.flags = 0;
	colourViewCI.image = images.back(); //image
	colourViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	colourViewCI.format = imageFormat; //image format.
	colourViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
	colourViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
	colourViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
	colourViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
	colourViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	colourViewCI.subresourceRange.baseMipLevel = 0;
	colourViewCI.subresourceRange.levelCount = 1;
	colourViewCI.subresourceRange.baseArrayLayer = 0;
	colourViewCI.subresourceRange.layerCount = 1;

	imageViews.push_back(VkImageView());
	vkCreateImageView(logicalDevice, &colourViewCI, NULL, &imageViews.back());


	// Depth buffer //
	if (hasDepthBuffer)
	{
		//image creation
		VkImageCreateInfo depthImageCI;
		depthImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImageCI.pNext = NULL;
		depthImageCI.flags = 0; //Additional flags are in VkImageCreateFlagBits
		depthImageCI.imageType = VkImageType::VK_IMAGE_TYPE_2D;
		depthImageCI.format = VK_FORMAT_D16_UNORM;
		depthImageCI.extent = VkExtent3D{ pixelWidth, pixelHeight, 1 };
		depthImageCI.mipLevels = 0;
		depthImageCI.arrayLayers = 1;
		depthImageCI.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		depthImageCI.tiling = VkImageTiling::VK_IMAGE_TILING_LINEAR; //TODO: dig into what these mean.
		depthImageCI.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depthImageCI.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
		depthImageCI.queueFamilyIndexCount = 0;
		depthImageCI.pQueueFamilyIndices = NULL; //Ignorable because sharingMode is not concurrent.
		depthImageCI.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		images.push_back(VkImage());
		vkCreateImage(logicalDevice, &depthImageCI, NULL, &images.back());

		//image view creation
		VkImageViewCreateInfo depthViewCI;
		depthViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthViewCI.pNext = NULL;
		depthViewCI.flags = 0;
		depthViewCI.image = images.back(); //image
		depthViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthViewCI.format = VK_FORMAT_D16_UNORM; //image format.
		depthViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
		depthViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
		depthViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
		depthViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
		depthViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depthViewCI.subresourceRange.baseMipLevel = 0;
		depthViewCI.subresourceRange.levelCount = 1;
		depthViewCI.subresourceRange.baseArrayLayer = 0;
		depthViewCI.subresourceRange.layerCount = 1;

		imageViews.push_back(VkImageView());
		vkCreateImageView(logicalDevice, &depthViewCI, NULL, &imageViews.back());

		VkClearValue clearDepthValue;
		clearDepthValue.depthStencil.depth = 1.0;
		clearDepthValue.depthStencil.stencil = 0;
		clearValues.push_back(clearDepthValue);

	}
}

