#pragma once

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "GraphicsInstance.h"
#include "GraphicsPhysicalDevice.h"
#include "GraphicsLogicalDevice.h"

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

public:
	void SubmitGraphicsJob(VkCommandBuffer graphicsJob);


	GraphicsInstance* GetInstance() const				{ return instance; }
	GraphicsPhysicalDevice* GetPhysicalDevice() const	{ return physicalDevice; }
	GraphicsLogicalDevice* GetLogicalDevice() const		{ return logicalDevice; }


};

