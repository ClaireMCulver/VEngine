#pragma once

//Cpp Core
#include <vector>

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

class GraphicsInstance
{
public:
	GraphicsInstance();
	~GraphicsInstance();

	VkInstance GetInstance() const { return vkInstance; }

private:
	VkInstance vkInstance;

	std::vector<const char*> instanceLayers;

	std::vector<const char*> instanceExtentions; 

	VkDebugReportCallbackEXT debugCallback;

	private:
	bool ValidationLayersAreSupported(std::vector<const char*> &validationLayers);

};