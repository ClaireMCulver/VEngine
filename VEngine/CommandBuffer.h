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

	//Adds external semaphore to wait on
	void AddWaitSemaphore(VkSemaphore semaphore);

	//Adds external semaphopre to signal when completed.
	void AddSignalSemaphore(VkSemaphore semaphore);

	void SetDestinationStageMask(VkPipelineStageFlags stageMask);

	//Blocks until the command buffer has finished execution. Do not use this in conjunction with ResetBuffer, as this is called within it.
	void WaitForCompletion();


private:
	VkCommandBuffer vkCommandBuffer;
	VkSubmitInfo submitInfo;
	VkQueue submitQueue;
	VkCommandBufferBeginInfo beginInfo;

	CommandPool* pCommandPool;
	VkFence vkFence;

	std::vector<VkSemaphore> waitSemaphores;
	std::vector<VkSemaphore> signalSemaphores;
	VkPipelineStageFlags dstStageMask;

	//Reference to the logical device to which the command buffer belongs. I don't want to have all the calls each frame to get it.
	VkDevice logicalDevice;
};

