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
#include "GameObject.h"

#include "Camera.h"

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
	void ResetBuffer();

	VkRenderPass GetVKRenderPass() const { return renderPass; }
	VkDescriptorSetLayout GetVKDescriptorSetLayout() const { return descriptorSetLayout[0]; }
	VkPipelineLayout GetVKPipelineLayout() const { return pipelineLayout; }

	void RegisterObject(GameObject *object, int subpass);

	//Gets the first image in the images vector. BUFFER MUST HAVE ALREADY COMPLETED. THIS IS NOT ASSURED BY THIS FUNCTION. //TODO: Should change this some time later on to specify the image we want.
	Image* GetRenderedImage();

private:
	VkRenderPass renderPass;
	VkRenderPassBeginInfo renderPassBeginInfo;

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

	std::vector<std::vector<GameObject*>*> registeredMeshes;

	VkPresentInfoKHR bufferPresentInfo;

	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout[2];
	VkDescriptorSet descriptorSet;

	UniformBuffer* perFrameUniformBuffer;

private:
	void CreateImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat imageFormat, bool hasDepthBuffer);
};

