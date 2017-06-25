#include "Graphics.h"



GraphicsSystem::GraphicsSystem()
{
	instance = new GraphicsInstance();
	physicalDevice = new GraphicsPhysicalDevice(*instance);
	logicalDevice = new GraphicsLogicalDevice(*physicalDevice);

	const VkDevice vkDevice = logicalDevice->GetLogicalDevice();

	GraphicsPhysicalDevice::QueueFamilyIndices indices = physicalDevice->GetQueueFamilyIndices();

	vkGetDeviceQueue(vkDevice, indices.graphicsQueueIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(vkDevice, indices.computeQueueIndex, 0, &computeQueue);
	vkGetDeviceQueue(vkDevice, indices.transferQueueIndex, 0, &transferQueue);
	vkGetDeviceQueue(vkDevice, indices.sparseQueueIndex, 0, &sparseQueue);
}


GraphicsSystem::~GraphicsSystem()
{
	delete logicalDevice;
	delete physicalDevice;
	delete instance;
}

void GraphicsSystem::SubmitGraphicsJob(VkCommandBuffer graphicsJob, VkSemaphore* pWaitSemaphores = NULL, uint32_t waitSemaphoreCount = 0, VkSemaphore* pSignalSemaphores = NULL, uint32_t signalSemaphoreCount = 0)
{
	VkSubmitInfo graphicsInfo;
	graphicsInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	graphicsInfo.pNext = NULL;
	graphicsInfo.waitSemaphoreCount = waitSemaphoreCount;
	graphicsInfo.pWaitSemaphores = pWaitSemaphores;
	VkPipelineStageFlags pipelineStageFlag = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	graphicsInfo.pWaitDstStageMask = &pipelineStageFlag;
	graphicsInfo.commandBufferCount = 1;
	graphicsInfo.pCommandBuffers = &graphicsJob;
	graphicsInfo.signalSemaphoreCount = signalSemaphoreCount;
	graphicsInfo.pSignalSemaphores = pSignalSemaphores;

	vkQueueSubmit(graphicsQueue, 1, &graphicsInfo, NULL);
}

void GraphicsSystem::SubmitTransferJob(VkCommandBuffer transferJob, VkSemaphore* pWaitSemaphores = NULL, uint32_t waitSemaphoreCount = 0, VkSemaphore* pSignalSemaphores = NULL, uint32_t signalSemaphoreCount = 0)
{
	VkSubmitInfo transferInfo;
	transferInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	transferInfo.pNext = NULL;
	transferInfo.waitSemaphoreCount = waitSemaphoreCount;
	transferInfo.pWaitSemaphores = pWaitSemaphores;
	VkPipelineStageFlags pipelineStageFlag = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
	transferInfo.pWaitDstStageMask = &pipelineStageFlag;
	transferInfo.commandBufferCount = 1;
	transferInfo.pCommandBuffers = &transferJob;
	transferInfo.signalSemaphoreCount = signalSemaphoreCount;
	transferInfo.pSignalSemaphores = pSignalSemaphores;

	vkQueueSubmit(transferQueue, 1, &transferInfo, NULL);
}