#pragma once

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "GraphicsInstance.h"
#include "GraphicsPhysicalDevice.h"
#include "GraphicsLogicalDevice.h"

#include "CommandPool.h"
#include "CommandBuffer.h"

class GraphicsSystem
{
public:
	GraphicsSystem();
	~GraphicsSystem();

private:
	GraphicsInstance* instance;
	GraphicsPhysicalDevice* physicalDevice;
	GraphicsLogicalDevice* logicalDevice;

	//Job queues
	VkQueue graphicsQueue;
	VkQueue computeQueue;
	VkQueue transferQueue;
	VkQueue sparseQueue;

	//Command buffer pools
	//TODO: Do I really want these here? or somewhere else?
	CommandPool* graphicsCmdPool;
	CommandPool* computeCmdPool;
	CommandPool* transferCmdPool;
	CommandPool* sparseCmdPool;

public:
	void SubmitGraphicsJob(CommandBuffer graphicsJob);
	void SubmitGraphicsJob(VkCommandBuffer graphicsJob, VkSemaphore* pWaitSemaphores = NULL, uint32_t waitSemaphoreCount = 0, VkSemaphore* pSignalSemaphores = NULL, uint32_t signalSemaphoreCount = 0);
	
	void SubmitTransferJob(CommandBuffer transferJob);
	void SubmitTransferJob(VkCommandBuffer transferJob, VkSemaphore* pWaitSemaphores = NULL, uint32_t waitSemaphoreCount = 0, VkSemaphore* pSignalSemaphores = NULL, uint32_t signalSemaphoreCount = 0);


	GraphicsInstance* GetInstance() const				{ return instance; }
	GraphicsPhysicalDevice* GetPhysicalDevice() const	{ return physicalDevice; }
	GraphicsLogicalDevice* GetLogicalDevice() const		{ return logicalDevice; }


	VkQueue GetGraphicsQueue() const	{ return graphicsQueue; }
	VkQueue GetComputeQueue() const		{ return computeQueue; }
	VkQueue GetTransferQueue() const	{ return transferQueue; }
	VkQueue GetSparseQueue() const		{ return sparseQueue; }
};

