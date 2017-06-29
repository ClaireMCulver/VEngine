#include "CommandBuffer.h"



CommandBuffer::CommandBuffer(CommandPool commandPool, VkCommandBufferLevel bufferLevel)
{
	pCommandPool = &commandPool;

	// Allocation //
	VkCommandBufferAllocateInfo allocationInfo;
	allocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocationInfo.pNext = NULL;
	allocationInfo.commandPool = commandPool.GetVKCommandPool();
	allocationInfo.level = bufferLevel;
	allocationInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(pCommandPool->GetVkLogicalDevice(), &allocationInfo, &vkCommandBuffer);
	
	// Submit info //
	vkBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkBufferSubmitInfo.pNext = 0;
	vkBufferSubmitInfo.waitSemaphoreCount = 0;
	vkBufferSubmitInfo.pWaitSemaphores = NULL;
	vkBufferSubmitInfo.pWaitDstStageMask = 0;
	vkBufferSubmitInfo.commandBufferCount = 1;
	vkBufferSubmitInfo.pCommandBuffers = &vkCommandBuffer;
	vkBufferSubmitInfo.signalSemaphoreCount = 0;
	vkBufferSubmitInfo.pSignalSemaphores = NULL;
}


CommandBuffer::~CommandBuffer()
{
	vkFreeCommandBuffers(pCommandPool->GetVkLogicalDevice(), pCommandPool->GetVKCommandPool(), 1, &vkCommandBuffer);
}