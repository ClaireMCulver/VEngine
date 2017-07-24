#include "CommandBuffer.h"



CommandBuffer::CommandBuffer(CommandBufferType bufferType, VkCommandBufferLevel bufferLevel)
{
	switch (bufferType)
	{
	case Graphics:
		submitQueue = GraphicsSystem::GetSingleton()->GetGraphicsQueue();
		pCommandPool = GraphicsSystem::GetSingleton()->GetGraphicsCommandPool();
		break;
	case Compute:
		submitQueue = GraphicsSystem::GetSingleton()->GetComputeQueue();
		pCommandPool = GraphicsSystem::GetSingleton()->GetComputeCommandPool();
		break;
	case Transfer:
		submitQueue = GraphicsSystem::GetSingleton()->GetTransferQueue();
		pCommandPool = GraphicsSystem::GetSingleton()->GetTransferCommandPool();
		break;
	case Sparse:
		submitQueue = GraphicsSystem::GetSingleton()->GetSparseQueue();
		pCommandPool = GraphicsSystem::GetSingleton()->GetSparseCommandPool();
		break;
	default:
		submitQueue = GraphicsSystem::GetSingleton()->GetGraphicsQueue();
		pCommandPool = GraphicsSystem::GetSingleton()->GetGraphicsCommandPool();
		break;
	}

	// Allocation //
	VkCommandBufferAllocateInfo allocationInfo;
	allocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocationInfo.pNext = NULL;
	allocationInfo.commandPool = pCommandPool->GetVKCommandPool();
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
	submitInfo.pWaitDstStageMask = &dstStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vkCommandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;
}

CommandBuffer::~CommandBuffer()
{
	vkQueueWaitIdle(submitQueue);

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

void CommandBuffer::SubmitBuffer()
{
	vkQueueSubmit(submitQueue, 1, &submitInfo, NULL);
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

void CommandBuffer::SetDestinationStageMask(VkPipelineStageFlags stageMask)
{
	dstStageMask = stageMask;
}