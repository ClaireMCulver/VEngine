#include "GraphicsLogicalDevice.h"

GraphicsLogicalDevice::GraphicsLogicalDevice(GraphicsPhysicalDevice &physicalDevice)
{
	VkDeviceCreateInfo logicalDeviceCI;
	logicalDeviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCI.pNext = nullptr;
	logicalDeviceCI.flags = 0;
	logicalDeviceCI.queueCreateInfoCount = 0;
	logicalDeviceCI.pQueueCreateInfos = nullptr;
	logicalDeviceCI.enabledLayerCount = 0;
	logicalDeviceCI.ppEnabledLayerNames;
	logicalDeviceCI.enabledExtensionCount;
	logicalDeviceCI.ppEnabledExtensionNames;
	logicalDeviceCI.pEnabledFeatures;

	vkCreateDevice(physicalDevice.GetPhysicalDevice(), &logicalDeviceCI, nullptr, &logicalDevice);

	//GraphicsPhysicalDevice::QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();
}


GraphicsLogicalDevice::~GraphicsLogicalDevice()
{
}
