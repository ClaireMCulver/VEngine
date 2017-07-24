#pragma once

#include <vector>

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#include "Graphics.h"
#include "CommandPool.h"

enum CommandBufferType
{
	Graphics = 0,
	Compute = 1,
	Transfer = 2,
	Sparse = 3
};

class CommandBuffer
{
public:
	CommandBuffer(CommandBufferType bufferType, VkCommandBufferLevel bufferLevel);
	~CommandBuffer();

	VkCommandBuffer GetVKCommandBuffer() const { return vkCommandBuffer; }
	VkSubmitInfo GetSubmitInfo() const { return submitInfo; }
	VkQueue GetSubmitQueue() const { return submitQueue; }

	//Begin recording of the buffer
	void BeginRecording();

	//End the recording of the buffer
	void EndRecording();

	//Submits the buffer to the job system for work.
	void SubmitBuffer();

	//Reset the buffer to begin recording again
	void ResetBuffer();

	//TODO: How the fuck do I want to do this?
	void AddWaitSemaphore(VkSemaphore semaphore);
	//TODO: How the fuck do I want to do this?
	void AddSignalSemaphore(VkSemaphore semaphore);

	void SetFence(VkFence* fence) { vkFence = fence; }

	void SetDestinationStageMask(VkPipelineStageFlags stageMask);


private:
	VkCommandBuffer vkCommandBuffer;
	VkSubmitInfo submitInfo;
	VkQueue submitQueue;
	VkCommandBufferBeginInfo beginInfo;

	CommandPool* pCommandPool;
	VkFence* vkFence = NULL;

	std::vector<VkSemaphore> waitSemaphores;
	std::vector<VkSemaphore> signalSemaphores;
	VkPipelineStageFlags dstStageMask;
};

