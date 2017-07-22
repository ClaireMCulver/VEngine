#include "Graphics.h"

GraphicsSystem* GraphicsSystem::singleton = NULL;

GraphicsSystem::GraphicsSystem()
{
	instance = new GraphicsInstance();
	physicalDevice = new GraphicsPhysicalDevice(*instance);
	logicalDevice = new GraphicsLogicalDevice(*physicalDevice);

	const VkDevice vkDevice = logicalDevice->GetVKLogicalDevice();

	GraphicsPhysicalDevice::QueueFamilyIndices indices = physicalDevice->GetQueueFamilyIndices();

	vkGetDeviceQueue(vkDevice, indices.graphicsQueueIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(vkDevice, indices.computeQueueIndex, 0, &computeQueue);
	vkGetDeviceQueue(vkDevice, indices.transferQueueIndex, 0, &transferQueue);
	vkGetDeviceQueue(vkDevice, indices.sparseQueueIndex, 0, &sparseQueue);

	graphicsCmdPool = new CommandPool(*logicalDevice, indices.graphicsQueueIndex);
	computeCmdPool  = new CommandPool(*logicalDevice, indices.computeQueueIndex);
	transferCmdPool = new CommandPool(*logicalDevice, indices.transferQueueIndex);
	sparseCmdPool   = new CommandPool(*logicalDevice, indices.sparseQueueIndex);

	assert(singleton == NULL);
	singleton = this;
}


GraphicsSystem::~GraphicsSystem()
{
	delete graphicsCmdPool;
	delete computeCmdPool;
	delete transferCmdPool;
	delete sparseCmdPool;

	delete logicalDevice;
	delete physicalDevice;
	delete instance;
}