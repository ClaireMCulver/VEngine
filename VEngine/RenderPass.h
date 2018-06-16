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

#include "ObjectManager.h"
#include "CommandBuffer.h"
#include "GameObject.h"

struct SubpassReferences
{
	std::vector<VkAttachmentReference> inputReferences;
	std::vector<VkAttachmentReference> colourReferences;
	std::vector<VkAttachmentReference> resolveReferences;
	std::vector<VkAttachmentReference> preserveReferences;
	VkAttachmentReference depthReference = { 0, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED };
};

class RenderPass
{
public:
	RenderPass(uint32_t renderAreaWidth, uint32_t renderAreaHeight);
	~RenderPass();

	//Adds new subpass to renderpass
	void AddNewSubPass(); 

	//Adds a new colour attachement to the subpass.
	//Returns a VkAttachementReference to the new attachment for input references.
	VkAttachmentReference AddColourAttachmentToCurrentSubpass(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat renderbufferFormat);

	//Adds a depth attachement to the current Subpass
	void AddDepthAttachmentToCurrentSubpass(uint32_t pixelWidth, uint32_t pixelHeight);

	//Adds the given attachment as an input to the current subpass.
	void AddInputAttachmentToCurrentSubpass(VkAttachmentReference attachment);

	void CreateRenderPass();

	void BindRenderPass(VkCommandBuffer &cmdBuffer);
	void UnbindRenderPass(VkCommandBuffer &cmdBuffer);

	void RecordBuffer(glm::mat4 &vMatrix, glm::mat4 &pMatrix, glm::mat4 &vpMatrix);
	void SubmitBuffer();
	void ResetBuffer();

	VkRenderPass GetVKRenderPass() const { return renderPass; }
	VkDescriptorSetLayout GetVKDescriptorSetLayout() const { return descriptorSetLayout[0]; }
	VkPipelineLayout GetVKPipelineLayout() const { return pipelineLayout; }

	//Gets the first image in the images vector. BUFFER MUST HAVE ALREADY COMPLETED. THIS IS NOT ASSURED BY THIS FUNCTION.
	Image* GetImage(int index);

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

	VkPresentInfoKHR bufferPresentInfo;

	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout[2];
	VkDescriptorSet descriptorSet;

	UniformBuffer* perFrameUniformBuffer;

private:
	void CreateColourImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight, VkFormat imageFormat);
	void CreateDepthImageAndImageView(uint32_t pixelWidth, uint32_t pixelHeight);
};

