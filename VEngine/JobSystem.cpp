#include "JobSystem.h"



JobSystem::JobSystem()
{
}


JobSystem::~JobSystem()
{

}

void JobSystem::SubmitGraphicsJob(CommandBuffer graphicsJob)
{
	const VkSubmitInfo submitInfo = graphicsJob.GetSubmitInfo();
	vkQueueSubmit(GraphicsSystem::GetSingleton()->GetGraphicsQueue(), 1, &submitInfo, NULL);
}

void JobSystem::SubmitGraphicsJob(VkCommandBuffer graphicsJob, VkSemaphore* pWaitSemaphores, uint32_t waitSemaphoreCount, VkSemaphore* pSignalSemaphores, uint32_t signalSemaphoreCount)
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

	vkQueueSubmit(GraphicsSystem::GetSingleton()->GetGraphicsQueue(), 1, &graphicsInfo, NULL);
}

void JobSystem::SubmitTransferJob(CommandBuffer transferJob)
{
	const VkSubmitInfo submitInfo = transferJob.GetSubmitInfo();
	vkQueueSubmit(GraphicsSystem::GetSingleton()->GetTransferQueue(), 1, &submitInfo, NULL);
}

void JobSystem::SubmitTransferJob(VkCommandBuffer transferJob, VkSemaphore* pWaitSemaphores, uint32_t waitSemaphoreCount, VkSemaphore* pSignalSemaphores, uint32_t signalSemaphoreCount)
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

	vkQueueSubmit(GraphicsSystem::GetSingleton()->GetTransferQueue(), 1, &transferInfo, NULL);
}