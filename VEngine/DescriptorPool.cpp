#include "DescriptorPool.h"

DescriptorPool* DescriptorPool::singleton = NULL;

DescriptorPool::DescriptorPool()
{
	//TODO: figure out a way to manage the number of needed descriptors. I don't like doing this hard coded.
	std::vector<VkDescriptorPoolSize> typeCount;
	typeCount.resize(2);
	typeCount[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	typeCount[0].descriptorCount = 32;
	typeCount[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	typeCount[1].descriptorCount = 32;

	VkDescriptorPoolCreateInfo descPoolCI;
	descPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descPoolCI.pNext = NULL;
	descPoolCI.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descPoolCI.maxSets = 32;
	descPoolCI.poolSizeCount = (uint32_t)typeCount.size();
	descPoolCI.pPoolSizes = typeCount.data();

	vkCreateDescriptorPool(GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice(), &descPoolCI, NULL, &vkDescriptorPool);

	assert(singleton == NULL);
	singleton = this;
}


DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice(), vkDescriptorPool, NULL);
}
