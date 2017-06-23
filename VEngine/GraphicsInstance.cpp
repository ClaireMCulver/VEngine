#include "GraphicsInstance.h"




//Debug layer callbacks
#ifdef _DEBUG
#include <iostream>
VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  objectType,
	uint64_t                    object,
	size_t                      location,
	int32_t                     messageCode,
	const char*                 pLayerPrefix,
	const char*                 pMessage,
	void*                       pUserData)
{
	std::cerr << pMessage << std::endl;
	return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

#endif

GraphicsInstance::GraphicsInstance()
{
	VkResult result;

	// Application and Engine information //
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = GameName;
	appInfo.applicationVersion = GameVersion;
	appInfo.pEngineName = EngineName;
	appInfo.engineVersion = EngineVersion;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Enabled instance layers //
#ifdef _DEBUG
	instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation"); //Not sure what the defined name here is.

	assert(ValidationLayersAreSupported(instanceLayers));
#endif

	// Enabled instance extentions //
	instanceExtentions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceExtentions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#ifdef  _DEBUG
	instanceExtentions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif //  _DEBUG

	VkInstanceCreateInfo instanceCI;
	instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCI.pNext = nullptr;
	instanceCI.flags = 0;
	instanceCI.pApplicationInfo = &appInfo;
	instanceCI.enabledLayerCount = instanceLayers.size();
	instanceCI.ppEnabledLayerNames = instanceLayers.data();
	instanceCI.enabledExtensionCount = instanceExtentions.size();
	instanceCI.ppEnabledExtensionNames = instanceExtentions.data();

	result = vkCreateInstance(&instanceCI, NULL, &vkInstance);
	assert(result == VK_SUCCESS);

#ifdef _DEBUG
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = DebugReportCallback;

	result = CreateDebugReportCallbackEXT(vkInstance, &createInfo, nullptr, &debugCallback);
	assert(result == VK_SUCCESS);
#endif
}


GraphicsInstance::~GraphicsInstance()
{
#ifdef _DEBUG
	DestroyDebugReportCallbackEXT(vkInstance, debugCallback, NULL);
#endif // _DEBUG
	
	vkDestroyInstance(vkInstance, NULL);	
}



bool GraphicsInstance::ValidationLayersAreSupported(std::vector<const char*> &validationLayers)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}