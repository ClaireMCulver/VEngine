#include "GraphicsPhysicalDevice.h"



GraphicsPhysicalDevice::GraphicsPhysicalDevice(GraphicsInstance &instance)
{
	// Fetch the number of physical devices //
	uint32_t numberOfDevices;
	vkEnumeratePhysicalDevices(instance.GetInstance(), &numberOfDevices, nullptr);

	// Reserve the same number of elements in the arrays //
	physicalDevices.resize(numberOfDevices);
	physicalDeviceProperties.resize(numberOfDevices);
	physicalDeviceFeatures.resize(numberOfDevices);
	physicalDeviceMemoryProperties.resize(numberOfDevices);
	physicalDeviceQueueFamilyProperties.resize(numberOfDevices);
	physicalDeviceQueueFamilyIndices.resize(numberOfDevices);

	// Physical devices themselves //
	vkEnumeratePhysicalDevices(instance.GetInstance(), &numberOfDevices, physicalDevices.data());

	// Physical device properties //
	for (size_t i = 0, count = physicalDevices.size(); i < count; i++)
	{
		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties[i]);
	}
	
	// Physical device features //
	for (size_t i = 0, count = physicalDevices.size(); i < count; i++)
	{
		vkGetPhysicalDeviceFeatures(physicalDevices[i], &physicalDeviceFeatures[i]);
	}

	// Physical device memory properties //
	for (size_t i = 0, count = physicalDevices.size(); i < count; i++)
	{
		vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &physicalDeviceMemoryProperties[i]);
	}

	// Physical device queue family properties //
	for (size_t i = 0, count = physicalDevices.size(); i < count; i++)
	{
		uint32_t numberOfQueueFamilies;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &numberOfQueueFamilies, nullptr);
		physicalDeviceQueueFamilyProperties[i].resize(numberOfQueueFamilies);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &numberOfQueueFamilies, physicalDeviceQueueFamilyProperties[i].data());
	}

	GetQueueFamilyIndices();
}


GraphicsPhysicalDevice::~GraphicsPhysicalDevice()
{
}

void GraphicsPhysicalDevice::GetQueueFamilyIndices()
{
	bool graphicsFound = false, computeFound = false, transferFound = false, sparseFound = false;
	//I've decided to just get the queues from the first device for now. There's just no point for now in having multiple queues from multiple devices.
	for (size_t i = 0, count = 1/* physicalDeviceQueueFamilyIndices.size()*/; i < count; i++)
	{
		for (size_t j = 0, countb = physicalDeviceQueueFamilyProperties[i].size(); j < countb; j++)
		{
			bool queueFound = false;
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].graphicsQueueIndex = j;
				graphicsFound = true;
				queueFound = true;
			}
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].computeQueueIndex = j;
				computeFound = true;
				queueFound = true;
			}
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].transferQueueIndex = j;
				transferFound = true;
				queueFound = true;
			}
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_SPARSE_BINDING_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].sparseQueueIndex = j;
				sparseFound = true;
				queueFound = true;
			}
			if (queueFound)
			{
				queuesNeeded++;
			}
			if (graphicsFound == true && computeFound == true && transferFound == true && sparseFound == true)
			{
				break;
			}
		}
	}
}