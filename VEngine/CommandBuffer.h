#pragma once

#include <vector>

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>

#include "CommandPool.h"

class CommandBuffer
{
public:
	CommandBuffer(CommandPool commandPool, VkCommandBufferLevel bufferLevel);
	~CommandBuffer();

	VkCommandBuffer GetVKCommandBuffer() const { return vkCommandBuffer; }

	//TODO: How the fuck do I want to do these?
	void BeginRecording();
	void EndRecording();
	void SubmitBuffer();
	void AddWaitSemaphore();
	void AddSignalSemaphore();


private:
	VkCommandBuffer vkCommandBuffer;
	VkSubmitInfo vkBufferSubmitInfo;

	CommandPool* pCommandPool;

	std::vector<VkSemaphore> waitSemaphores;
	std::vector<VkSemaphore> signalSemaphores;
};

