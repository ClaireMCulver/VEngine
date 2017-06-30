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
	
	// Begin Info //
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = NULL;

	// Submit info //
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = 0;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = 0;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vkCommandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;
}


CommandBuffer::~CommandBuffer()
{
	vkFreeCommandBuffers(pCommandPool->GetVkLogicalDevice(), pCommandPool->GetVKCommandPool(), 1, &vkCommandBuffer);
}

void CommandBuffer::BeginRecording()
{
	vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
}

void CommandBuffer::EndRecording()
{
	vkEndCommandBuffer(vkCommandBuffer);
}

void CommandBuffer::ResetBuffer()
{
	vkResetCommandBuffer(vkCommandBuffer, 0);
}

void CommandBuffer::AddWaitSemaphore(VkSemaphore semaphore)
{
	waitSemaphores.push_back(semaphore);

	submitInfo.waitSemaphoreCount = waitSemaphores.size();
	submitInfo.pWaitSemaphores = waitSemaphores.data();
}

void CommandBuffer::AddSignalSemaphore(VkSemaphore semaphore)
{
	signalSemaphores.push_back(semaphore);

	submitInfo.signalSemaphoreCount = signalSemaphores.size();
	submitInfo.pSignalSemaphores = signalSemaphores.data();
}