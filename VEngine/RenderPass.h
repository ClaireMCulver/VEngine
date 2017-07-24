#pragma once

//cpp core
#include <vector>

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

//Engine
#include "Graphics.h"
#include "Image.h"
#include "FrameBuffer.h"

#include "CommandBuffer.h"
#include "RenderableObject.h"

struct SubpassReferences
{
	std::vector<VkAttachmentReference> colourReferences;
	VkAttachmentReference depthReference = { 0, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED };
};

class RenderPass
{
public:
	RenderPass();
	~RenderPass();

	void AddNewSubPass(); 
	void AddColourAttachementToCurrentSubpass(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat renderbufferFormat, bool useDepthBuffer);

	void CreateRenderPass();

	void BindRenderPass(VkCommandBuffer &cmdBuffer);
	void UnbindRenderPass(VkCommandBuffer &cmdBuffer);

	void RecordBuffer();
	void SubmitBuffer();

	VkRenderPass GetVKRenderPass() const { return renderPass; }

	void RegisterObject(RenderableObject &object);

private:
	VkRenderPass renderPass;
	VkRenderPassBeginInfo renderPassBeginInfo;

	VkSemaphore renderFinishedSemaphore;
	CommandBuffer* renderBuffer;

	std::vector<VkSubpassDescription> subpassDescriptions; //descriptions of the subpasses themselves.
	std::vector<SubpassReferences> subpassReferences;//references to descriptions for each subpass.
	std::vector<Image*> images;
	std::vector<VkImageView> vkImageViews; //views of the images for the frame buffer
	std::vector<VkAttachmentDescription> attachmentDescriptions; //descriptions of the attachement for the renderpass itself
	VkFramebuffer frameBuffer;

	VkViewport viewport;
	VkRect2D renderArea;
	std::vector<VkClearValue> clearValues;

	std::vector<RenderableObject*> registeredMeshes;

	VkPresentInfoKHR bufferPresentInfo;

private:
	void CreateImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat imageFormat, bool hasDepthBuffer);
};

