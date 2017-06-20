#include "GraphicsLogicalDevice.h"

GraphicsLogicalDevice::GraphicsLogicalDevice(GraphicsPhysicalDevice &physicalDevice)
{
	//Fetch info on the device's queues
	GraphicsPhysicalDevice::QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();

	VkDeviceQueueCreateInfo queuesRequested;
	queuesRequested.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queuesRequested.pNext = nullptr;
	queuesRequested.flags = 0;
	queuesRequested.queueFamilyIndex = indices.graphicsQueueIndex; //some method in order to create all the necesarry queues. Sould probably go back and change the QueueFamilyIndices to contain all the necessary info from te fetching of the information.
	queuesRequested.queueCount = 1;// indices.queuesNeeded;
	float graphicsPriority = 0.0f;
	queuesRequested.pQueuePriorities = &graphicsPriority; //Add in priorities for potential optimization


	VkDeviceCreateInfo logicalDeviceCI;
	logicalDeviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCI.pNext = nullptr;
	logicalDeviceCI.flags = 0;
	logicalDeviceCI.queueCreateInfoCount = 1;
	logicalDeviceCI.pQueueCreateInfos = &queuesRequested;
	logicalDeviceCI.enabledLayerCount = 0;
	logicalDeviceCI.ppEnabledLayerNames = nullptr;
	logicalDeviceCI.enabledExtensionCount = 0;
	logicalDeviceCI.ppEnabledExtensionNames = nullptr;
	logicalDeviceCI.pEnabledFeatures = nullptr;

	vkCreateDevice(physicalDevice.GetPhysicalDevice(), &logicalDeviceCI, nullptr, &logicalDevice);

	//Fetch the queues
}


GraphicsLogicalDevice::~GraphicsLogicalDevice()
{
	vkDeviceWaitIdle(logicalDevice);
	vkDestroyDevice(logicalDevice, nullptr);
}
