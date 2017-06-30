#include "CommandPool.h"



CommandPool::CommandPool(GraphicsLogicalDevice &logicalDevice, uint32_t commandBufferQueueFamily)
{
	VkCommandPoolCreateInfo commandPoolCI;
	commandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCI.pNext = NULL;
	commandPoolCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCI.queueFamilyIndex = commandBufferQueueFamily;

	vkLogicalDevice = logicalDevice.GetVKLogicalDevice();

	vkCreateCommandPool(vkLogicalDevice, &commandPoolCI, NULL, &vkCommandPool);
}


CommandPool::~CommandPool()
{
	vkDestroyCommandPool(vkLogicalDevice, vkCommandPool, NULL);
}