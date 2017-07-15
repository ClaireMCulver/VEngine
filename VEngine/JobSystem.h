#pragma once

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Graphics.h"
#include "CommandBuffer.h"

class JobSystem
{
public:
	JobSystem();
	~JobSystem();

	static void SubmitGraphicsJob(CommandBuffer graphicsJob);
	static void SubmitGraphicsJob(VkCommandBuffer graphicsJob, VkSemaphore* pWaitSemaphores = NULL, uint32_t waitSemaphoreCount = 0, VkSemaphore* pSignalSemaphores = NULL, uint32_t signalSemaphoreCount = 0);
	 
	static void SubmitTransferJob(CommandBuffer transferJob);
	static void SubmitTransferJob(VkCommandBuffer transferJob, VkSemaphore* pWaitSemaphores = NULL, uint32_t waitSemaphoreCount = 0, VkSemaphore* pSignalSemaphores = NULL, uint32_t signalSemaphoreCount = 0);

};

