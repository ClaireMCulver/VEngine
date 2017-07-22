#include "DescriptorPool.h"

DescriptorPool* DescriptorPool::singleton = NULL;

DescriptorPool::DescriptorPool()
{
	//TODO: figure out a way to manage the number of needed descriptors. I don't like doing this hard coded.
	std::vector<VkDescriptorPoolSize> typeCount;
	typeCount.resize(1);
	typeCount[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	typeCount[0].descriptorCount = 12;
	//typeCount[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//typeCount[1].descriptorCount = 12;

	VkDescriptorPoolCreateInfo descPoolCI;
	descPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descPoolCI.pNext = NULL;
	descPoolCI.flags = 0; //I do not plan to return any descriptors to the pool at any time, so flags == 0;
	descPoolCI.maxSets = 12;
	descPoolCI.poolSizeCount = typeCount.size();
	descPoolCI.pPoolSizes = typeCount.data();

	vkCreateDescriptorPool(GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice(), &descPoolCI, NULL, &vkDescriptorPool);

	assert(singleton == NULL);
	singleton = this;
}


DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice(), vkDescriptorPool, NULL);
}
