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

	// Fetch the discrete GPU's index in the physical device list //
	for (int i = 0, count = (int)physicalDeviceProperties.size(); i < count; i++)
	{
		if (physicalDeviceProperties[i].deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			discreteGPUIndex = i;
			break;
		}
	}
}


GraphicsPhysicalDevice::~GraphicsPhysicalDevice()
{
}

void GraphicsPhysicalDevice::FindQueueFamilyIndices()
{
	bool graphicsFound = false, computeFound = false, transferFound = false, sparseFound = false;
	//I've decided to just get the queues from the first device for now. There's just no point for now in having multiple queues from multiple devices.
	for (size_t i = 0, count = physicalDeviceQueueFamilyIndices.size(); i < count; i++)
	{
		for (size_t j = 0, countb = physicalDeviceQueueFamilyProperties[i].size(); j < countb; j++)
		{
			bool queueFound = false;
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].graphicsQueueIndex = (uint32_t)j;
				graphicsFound = true;
				queueFound = true;
			}
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].computeQueueIndex = (uint32_t)j;
				computeFound = true;
				queueFound = true;
			}
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].transferQueueIndex = (uint32_t)j;
				transferFound = true;
				queueFound = true;
			}
			if (physicalDeviceQueueFamilyProperties[i][j].queueFlags & VkQueueFlagBits::VK_QUEUE_SPARSE_BINDING_BIT)
			{
				physicalDeviceQueueFamilyIndices[i].sparseQueueIndex = (uint32_t)j;
				sparseFound = true;
				queueFound = true;
			}
			if (queueFound)
			{
				queuesNeeded++;
			}
			if (graphicsFound == true && computeFound == true && transferFound == true && sparseFound == true)
			{
				physicalDeviceQueueFamilyIndices[i].queuesNeeded = queuesNeeded;
				break;
			}
		}
	}
}