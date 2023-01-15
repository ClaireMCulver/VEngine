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

	void RecordBuffer(glm::mat4 &vMatrix, glm::mat4 &pMatrix, glm::mat4 &vpMatrix, std::vector<GameObject*>* objectList);
	void SubmitBuffer();
	void ResetBuffer();

	//Gets the first image in the images vector. BUFFER MUST HAVE ALREADY COMPLETED. THIS IS NOT ASSURED BY THIS FUNCTION.
	Image* GetImage(unsigned int index);

	VkRenderPass GetVKRenderPass() const { return renderPass; }
	VkDescriptorSetLayout GetVKDescriptorSetLayout() const { return descriptorSetLayout[0]; }
	VkPipelineLayout GetVKPipelineLayout() const { return pipelineLayout; }
	CommandBuffer* GetRenderBuffer() const { return renderBuffer; }
	size_t GetAttachmentsCount() const { return images.size() + (usesDepth ? -1 : 0); }

	void BeginPass(glm::mat4 &vMatrix, glm::mat4 &pMatrix, glm::mat4 &vpMatrix);
	void DrawObjects(std::vector<GameObject*>* objectList, glm::mat4 &vMatrix, glm::mat4 &vpMatrix);
	void EndPass();
protected:

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


private:
	VkRenderPass renderPass;
	VkRenderPassBeginInfo renderPassBeginInfo;

	CommandBuffer* renderBuffer;

	std::vector<VkSubpassDescription> subpassDescriptions; //descriptions of the subpasses themselves.
	std::vector<SubpassReferences> subpassReferences;//references to descriptions for each subpass.
	std::vector<Image*> images;
	std::vector<VkImageView> vkImageViews; //views of the images for the frame buffer
	std::vector<VkAttachmentDescription> attachmentDescriptions; //descriptions of the attachement for the renderpass itself
	bool usesDepth = false;
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

