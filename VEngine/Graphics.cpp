#include "Graphics.h"



Graphics::Graphics()
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


Graphics::~Graphics()
{
	delete logicalDevice;
	delete physicalDevice;
	delete instance;
}

void Graphics::SubmitGraphicsJob(VkCommandBuffer graphicsJob)
{
	VkSubmitInfo graphicsInfo;
	graphicsInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	graphicsInfo.pNext = NULL;
	graphicsInfo.waitSemaphoreCount = 0;
	graphicsInfo.pWaitSemaphores = NULL;
	VkPipelineStageFlags pipelineStageFlag = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	graphicsInfo.pWaitDstStageMask = &pipelineStageFlag;
	graphicsInfo.commandBufferCount = 1;
	graphicsInfo.pCommandBuffers = &graphicsJob;
	graphicsInfo.signalSemaphoreCount = 0;
	graphicsInfo.pSignalSemaphores = NULL;

	vkQueueSubmit(graphicsQueue, 1, &graphicsInfo, NULL);
}