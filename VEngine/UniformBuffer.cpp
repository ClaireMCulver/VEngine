#include "UniformBuffer.h"



UniformBuffer::UniformBuffer() : GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, PerDrawUniformBufferSize) 
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	vkMapMemory(logicalDevice, vkMemory, 0, PerDrawUniformBufferSize, 0, &ramBuffer);
}


UniformBuffer::~UniformBuffer()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	vkUnmapMemory(logicalDevice, vkMemory);
}

bool UniformBuffer::SetBufferData(void* data, size_t dataSize, int offset)
{
	if ((dataSize + offset) > PerDrawUniformBufferSize)
	{
		return false;
	}

	memcpy_s((BYTE*)ramBuffer + offset, PerDrawUniformBufferSize, data, dataSize); //Set the offset of the pointer based on the offset;

	return true;
}
