#pragma once

//Cpp Core
#include <vector>
#include <assert.h>

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

#include "GameDefs.h"
#include "VEngineDefs.h"

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