#pragma once

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <vector>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

#include "Win32Window.h"

class GraphicsSystem
{
public:
	GraphicsSystem();
	~GraphicsSystem();

	void Initialize();

	static VkDevice GetLogicalDevice() { return singleton->logicalDevice; };
	static VkPhysicalDevice GetPhysicalDevice() { return singleton->physicalDevices[0]; }

	static GraphicsSystem* GetSingleton()
	{
		return singleton;
	}
	
	inline VkPhysicalDeviceMemoryProperties GetPhysicalMemoryProperties() const { return memoryProperties; }
	inline VkDescriptorPool GetDescriptorPool() const { return descriptorPool; }
	inline VkCommandPool GetCommandPool() const { return commandPool; }
	inline VkQueue GetGraphicsQueue() const { return graphicsQueue; }

	bool PrepFrame();
	bool EndFrame();

	VkCommandBuffer commandBuffer;

private:

	static GraphicsSystem *singleton;
	std::vector<layer_properties> instance_layer_properties;

	std::vector<const char*> instanceExtensionNames;
	std::vector<const char*> deviceExtentionNames;
	std::vector<const char*> validationLayers;

	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkDevice logicalDevice;
	VkApplicationInfo appInfo;

	std::vector<VkPhysicalDevice> physicalDevices;
	VkPhysicalDeviceProperties deviceProperties;
	
	VkDeviceQueueCreateInfo queueInfo;
	uint32_t queueFamilyCount;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;
	VkPhysicalDeviceMemoryProperties memoryProperties;

	VkPipelineCache pipelineCache;
	VkCommandPool commandPool;
	VkDescriptorPool descriptorPool;



	Win32Window window;
	VkSurfaceKHR surface;
	glm::vec2 windowSize;

	VkFormat format;

	DepthBuffer depth;

	VkSwapchainKHR swapchain;
	uint32_t swapchainImageCount = 2; //I'm assuming two, double check the number of images used in the sample
	std::vector<SwapchainBuffer> swapchainBuffers;

	uint32_t current_buffer;
	VkSemaphore signalPresentationSemaphore;
	VkSemaphore imageAcquiredSemaphore;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkViewport viewport;
	VkRect2D scissor;

	//Core functions
	VkResult InitializeGlobalLayerProperties();
	VkResult init_global_extension_properties(layer_properties &layer_props);
	void InitializeInstanceExtensionNames();
	void InitializeDeviceExtensionNames();
	void InitializeValidationLayers();
	void InitializeInstance();
	void InitializeDebugCallback();
	void EnumeratePhysicalDevices();
	void InitializeLogicalDevice();
	void InitializeCommandPool();
	void InitializeCommandBuffer();
	void InitializeDeviceQueue();
	void InitializeSwapchainExtension();
	void InitializeSwapchain();
	void InitializeDepthBuffer();
	void InitializeDescriptorPool();
	void InitializePipelineCache();
	void InitializePresentationSemaphores();

	//Utility functions
	bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memoryProperties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
	void InitViewports();
	void InitScissors();
};