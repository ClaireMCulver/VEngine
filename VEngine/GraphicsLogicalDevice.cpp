#include "GraphicsLogicalDevice.h"

GraphicsLogicalDevice::GraphicsLogicalDevice(GraphicsPhysicalDevice &physicalDevice)
{
	//Fetch info on the device's queues
	GraphicsPhysicalDevice::QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();

	const VkPhysicalDeviceFeatures physicalDeviceFeatures = physicalDevice.GetPhysicalDeviceFeatures();

	VkDeviceQueueCreateInfo queuesRequested;
	queuesRequested.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queuesRequested.pNext = nullptr;
	queuesRequested.flags = 0;
	queuesRequested.queueFamilyIndex = indices.graphicsQueueIndex; //some method in order to create all the necesarry queues. Sould probably go back and change the QueueFamilyIndices to contain all the necessary info from te fetching of the information.
	queuesRequested.queueCount = 1;// indices.queuesNeeded;
	float graphicsPriority = 0.0f;
	queuesRequested.pQueuePriorities = &graphicsPriority; //Add in priorities for potential optimization

	// Device extentions //
	deviceExtentions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// Device creation //
	VkDeviceCreateInfo logicalDeviceCI;
	logicalDeviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCI.pNext = nullptr;
	logicalDeviceCI.flags = 0;
	logicalDeviceCI.queueCreateInfoCount = 1;
	logicalDeviceCI.pQueueCreateInfos = &queuesRequested;
	logicalDeviceCI.enabledLayerCount = 0;
	logicalDeviceCI.ppEnabledLayerNames = nullptr;
	logicalDeviceCI.enabledExtensionCount = (uint32_t)deviceExtentions.size();
	logicalDeviceCI.ppEnabledExtensionNames = deviceExtentions.data();
	logicalDeviceCI.pEnabledFeatures = &physicalDeviceFeatures;

	vkCreateDevice(physicalDevice.GetPhysicalDevice(), &logicalDeviceCI, nullptr, &vkLogicalDevice);
}


GraphicsLogicalDevice::~GraphicsLogicalDevice()
{
	vkDeviceWaitIdle(vkLogicalDevice);
	vkDestroyDevice(vkLogicalDevice, NULL);
}
