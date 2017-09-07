#pragma once

//Cpp Core
#include <vector>

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

#include "GraphicsInstance.h"


class GraphicsPhysicalDevice
{
public:
	GraphicsPhysicalDevice(GraphicsInstance &instance);
	~GraphicsPhysicalDevice();

	typedef struct QueueFamilyIndices
	{
		uint32_t graphicsQueueIndex;
		uint32_t computeQueueIndex;
		uint32_t transferQueueIndex;
		uint32_t sparseQueueIndex;
		uint32_t queuesNeeded;
	} QueueFamilyIndices;

	VkPhysicalDevice				 GetPhysicalDevice() const					{ return physicalDevices[discreteGPUIndex]; }
	VkPhysicalDeviceProperties		 GetPhysicalDeviceProperties() const		{ return physicalDeviceProperties[discreteGPUIndex]; }
	VkPhysicalDeviceFeatures		 GetPhysicalDeviceFeatures() const			{ return physicalDeviceFeatures[discreteGPUIndex]; }
	VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryPropertiess() const { return physicalDeviceMemoryProperties[discreteGPUIndex]; }
	VkQueueFamilyProperties*		 GetQueueFamilyProperties()					{ return physicalDeviceQueueFamilyProperties[discreteGPUIndex].data(); }
	QueueFamilyIndices				 GetQueueFamilyIndices() const				{ return physicalDeviceQueueFamilyIndices[discreteGPUIndex]; }



private:
	//The devices
	std::vector<VkPhysicalDevice> physicalDevices;

	//The properties of the devices
	std::vector<VkPhysicalDeviceProperties> physicalDeviceProperties;
	std::vector<VkPhysicalDeviceFeatures> physicalDeviceFeatures;
	std::vector<VkPhysicalDeviceMemoryProperties> physicalDeviceMemoryProperties;

	//All queue properties of each device
	std::vector<std::vector<VkQueueFamilyProperties>> physicalDeviceQueueFamilyProperties;
	
	std::vector<QueueFamilyIndices> physicalDeviceQueueFamilyIndices;

	int queuesNeeded = 0;

	int discreteGPUIndex = 0;
private:
	void FindQueueFamilyIndices();
};

