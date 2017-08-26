#include "RenderPass.h"

RenderPass::RenderPass()
{
	renderArea.extent = { 0, 0 };
	renderArea.offset = { 0, 0 };
}


RenderPass::~RenderPass()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	delete renderBuffer;

	vkDestroyFramebuffer(logicalDevice, frameBuffer, NULL);
	
	vkDestroyRenderPass(logicalDevice, renderPass, NULL);

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
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
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
	frameBufferCI.attachmentCount = vkImageViews.size();
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
	renderPassBeginInfo.clearValueCount = clearValues.size();
	renderPassBeginInfo.pClearValues = clearValues.data();
	
	//Allocate command buffer
	renderBuffer = new CommandBuffer(CommandBufferType::Graphics, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	renderBuffer->SetDestinationStageMask(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
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

	// Render pass contents //
	for (size_t i = 0, count = registeredMeshes.size(); i < count; i++)
	{
		registeredMeshes[i]->Draw(*renderBuffer);
	}

	//end render pass
	vkCmdEndRenderPass(vkRenderBuffer);

	//finish recording buffer
	renderBuffer->EndRecording();

}

void RenderPass::SubmitBuffer()
{
	renderBuffer->SubmitBuffer();
}

void RenderPass::ResetBuffer()
{
	renderBuffer->ResetBuffer();
}

void RenderPass::CreateImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat imageFormat, bool hasDepthBuffer)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	images.push_back(new Image(pixelWidth, pixelHeight, imageFormat, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT));
	images.back()->ChangeImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	if (hasDepthBuffer)
	{
		images.push_back(new Image(pixelWidth, pixelHeight, VkFormat::VK_FORMAT_D16_UNORM, VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT));
		images.back()->ChangeImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}

void RenderPass::RegisterObject(GameObject *object)
{
	registeredMeshes.push_back(object);
}

Image* RenderPass::GetRenderedImage()
{
	assert(images.size() > 0);
	
	return images[0];
}