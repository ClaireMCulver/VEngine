#pragma once

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "GraphicsInstance.h"
#include "GraphicsPhysicalDevice.h"
#include "GraphicsLogicalDevice.h"

#include "CommandBuffer.h"
#include "CommandPool.h"

class GraphicsSystem
{
public:
	GraphicsSystem();
	~GraphicsSystem();

private:
	static GraphicsSystem* singleton;

	GraphicsInstance* instance;
	GraphicsPhysicalDevice* physicalDevice;
	GraphicsLogicalDevice* logicalDevice;

	//TODO: Move this stuff into job system, so that graphics system is only for the dispatchaeble handles.
	//Job queues
	VkQueue graphicsQueue;
	VkQueue computeQueue;
	VkQueue transferQueue;
	VkQueue sparseQueue;

	//Command buffer pools
	CommandPool* graphicsCmdPool;
	CommandPool* computeCmdPool;
	CommandPool* transferCmdPool;
	CommandPool* sparseCmdPool;

public:
	static GraphicsSystem* GetSingleton()				{ return singleton; }
	GraphicsInstance* GetInstance() const				{ return instance; }
	GraphicsPhysicalDevice* GetPhysicalDevice() const	{ return physicalDevice; }
	GraphicsLogicalDevice* GetLogicalDevice() const		{ return logicalDevice; }


	VkQueue GetGraphicsQueue() const	{ return graphicsQueue; }
	VkQueue GetComputeQueue() const		{ return computeQueue; }
	VkQueue GetTransferQueue() const	{ return transferQueue; }
	VkQueue GetSparseQueue() const		{ return sparseQueue; }


	CommandPool* GetGraphicsCommandPool() const { return graphicsCmdPool; }
	CommandPool* GetComputeCommandPool() const { return computeCmdPool;  }
	CommandPool* GetTransferCommandPool() const { return transferCmdPool; }
	CommandPool* GetSparseCommandPool() const { return sparseCmdPool;   }

	void WaitForDeviceIdle();
};

