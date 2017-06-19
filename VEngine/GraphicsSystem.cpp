#include "GraphicsSystem.h"

GraphicsSystem* GraphicsSystem::singleton = NULL;

GraphicsSystem::GraphicsSystem()
{
	if (singleton != nullptr)
	{
		printf("More than one graphics system\n");
		exit(EXIT_FAILURE);
	}
	singleton = this;
	//Initialize Window Size
	windowSize.x = 800.0f;
	windowSize.y = 600.0f;
}

GraphicsSystem::~GraphicsSystem()
{	
	//Destroy descriptor pool
	vkDestroyDescriptorPool(logicalDevice, descriptorPool, NULL);

	//Destroy depth buffer
	vkDestroyImageView(logicalDevice, depth.view, NULL);
	vkDestroyImage(logicalDevice, depth.image, NULL);
	vkFreeMemory(logicalDevice, depth.mem, NULL);

	//Destroy swapchain
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		vkDestroyImageView(logicalDevice, swapchainBuffers[i].view, NULL);
	}
	vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);

	//Destroy command pool
	vkDestroyCommandPool(logicalDevice, commandPool, NULL);

	//Destroy window
	vkDestroySurfaceKHR(instance, surface, NULL);

	//Destroy device
	vkDeviceWaitIdle(logicalDevice);
	vkDestroyDevice(logicalDevice, NULL);

	//Destroy instance
	vkDestroyInstance(instance, NULL);
}

void GraphicsSystem::Initialize()
{
	InitializeGlobalLayerProperties();
	InitializeInstanceExtensionNames();
	InitializeDeviceExtensionNames();

	InitializeValidationLayers();

	InitializeInstance();

	InitializeDebugCallback();

	EnumeratePhysicalDevices();

	window.InitWindow("Dragons");

	InitializeSwapchainExtension();

	InitializeLogicalDevice();

	InitializeCommandPool();

	InitializeCommandBuffer();

	InitializeDeviceQueue();

	InitializeSwapchain();

	InitializeDepthBuffer();
	
	InitializeDescriptorPool();

	InitializePresentationSemaphores();
}

VkResult GraphicsSystem::InitializeGlobalLayerProperties()
{
	uint32_t instanceLayerCount;
	VkLayerProperties *vkProperties = nullptr;
	VkResult result;

	do
	{
		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);

		if (result)
		{
			return result;
		}

		if (instanceLayerCount == 0)
		{
			return VK_SUCCESS;
		}

		vkProperties = (VkLayerProperties *)realloc(vkProperties, instanceLayerCount * sizeof(VkLayerProperties));
	
		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, vkProperties);
	} while (result == VK_INCOMPLETE);

	for (uint32_t i = 0; i < instanceLayerCount; i++) {
		layer_properties layer_props;
		layer_props.properties = vkProperties[i];
		result = init_global_extension_properties(layer_props);
		if (result) return result;
		instance_layer_properties.push_back(layer_props);
	}
	free(vkProperties);
	return VK_SUCCESS;
}

VkResult GraphicsSystem::init_global_extension_properties(layer_properties &layer_props) {
	VkExtensionProperties *instance_extensions;
	uint32_t instance_extension_count;
	VkResult res;
	char *layer_name = NULL;

	layer_name = layer_props.properties.layerName;

	do {
		res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, NULL);
		if (res) return res;

		if (instance_extension_count == 0) {
			return VK_SUCCESS;
		}

		layer_props.extensions.resize(instance_extension_count);
		instance_extensions = layer_props.extensions.data();
		res = vkEnumerateInstanceExtensionProperties(layer_name, &instance_extension_count, instance_extensions);
	} while (res == VK_INCOMPLETE);

	return res;
}

void GraphicsSystem::InitializeInstanceExtensionNames()
{
	instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	instanceExtensionNames.push_back("VK_EXT_debug_report");
}

void GraphicsSystem::InitializeDeviceExtensionNames()
{
	deviceExtentionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void GraphicsSystem::InitializeValidationLayers()
{

	validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");

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

		assert(layerFound == true);
	}


}

void GraphicsSystem::InitializeInstance()
{
	VkResult result;

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "Dragons Test";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = "Kurenai";
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = validationLayers.size();
	instanceInfo.ppEnabledLayerNames = validationLayers.data();
	instanceInfo.enabledExtensionCount = (uint32_t)instanceExtensionNames.size();
	instanceInfo.ppEnabledExtensionNames = instanceExtensionNames.data();

	result = vkCreateInstance(&instanceInfo, NULL, &instance);
	if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
		std::cout << "cannot find a compatible Vulkan ICD\n";
		exit(-1);
	}
	else if (result) {
		printf("unkown error: %d\n", result);
		exit(-1);
	}

	return;
}

void GraphicsSystem::InitializeDebugCallback()
{
	/* Setup callback creation information */
	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
	callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	callbackCreateInfo.pNext = nullptr;
	callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	callbackCreateInfo.pUserData = nullptr;

	/* Register the callback */
}

void GraphicsSystem::EnumeratePhysicalDevices()
{
	VkResult result;
	uint32_t gpuCount;

	//Fetch the number of devices
	result = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
	//Allocation
	physicalDevices.resize(gpuCount);
	//Actually get the list
	result = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
	assert(result == VK_SUCCESS);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &queueFamilyCount, NULL);
	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &queueFamilyCount, queueFamilyProperties.data());
	assert(queueFamilyCount >= 1);

	/* This is as good a place as any to do this */
	vkGetPhysicalDeviceMemoryProperties(physicalDevices[0], &memoryProperties);
	vkGetPhysicalDeviceProperties(physicalDevices[0], &deviceProperties);
}

void GraphicsSystem::InitializeLogicalDevice()
{
	VkResult result;

	float queuePriorities[1] = { 0.0f };
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext = NULL;
	queueInfo.flags = 0;
	queueInfo.queueCount = 1;
	queueInfo.pQueuePriorities = queuePriorities;
	queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

	VkDeviceCreateInfo deviceInfo;
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = NULL;
	deviceInfo.flags = 0;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledLayerCount = validationLayers.size();
	deviceInfo.ppEnabledLayerNames = validationLayers.data();
	deviceInfo.enabledExtensionCount = deviceExtentionNames.size();
	deviceInfo.ppEnabledExtensionNames = deviceExtentionNames.data();
	deviceInfo.pEnabledFeatures = NULL;
	 
	result = vkCreateDevice(physicalDevices[0], &deviceInfo, NULL, &logicalDevice);
	assert(result == VK_SUCCESS);
}

void GraphicsSystem::InitializeCommandPool()
{
	VkResult result;

	VkCommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = NULL;
	cmdPoolInfo.queueFamilyIndex = queueInfo.queueFamilyIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	result = vkCreateCommandPool(logicalDevice, &cmdPoolInfo, NULL, &commandPool);
	assert(result == VK_SUCCESS);
}

void GraphicsSystem::InitializeCommandBuffer()
{
	VkResult result;

	VkCommandBufferAllocateInfo cmdBufferAllocateInfo;
	cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocateInfo.pNext = NULL;
	cmdBufferAllocateInfo.commandPool = commandPool;
	cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferAllocateInfo.commandBufferCount = 1;
	result = vkAllocateCommandBuffers(logicalDevice, &cmdBufferAllocateInfo, &commandBuffer);

	assert(result == VK_SUCCESS);
}

void GraphicsSystem::InitializeDeviceQueue()
{
	vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &graphicsQueue);
	if (graphicsQueueFamilyIndex == presentQueueFamilyIndex) {
		presentQueue = graphicsQueue;
	}
	else {
		vkGetDeviceQueue(logicalDevice, presentQueueFamilyIndex, 0, &presentQueue);
	}
}

void GraphicsSystem::InitializeSwapchainExtension()
{
	VkResult result;

	const WindowData winData = window.GetWindowData();

	VkWin32SurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.flags = 0;
	createInfo.pNext = NULL;
	createInfo.hinstance = winData.connection;
	createInfo.hwnd = winData.window;

	result = vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface);
	if (result != VK_SUCCESS)
	{
		printf("Error creating command buffer: %d", result);
		exit(1);
	}

	// Iterate over each queue to learn whether it supports presenting:
	VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], i, surface, &pSupportsPresent[i]);
	}

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	graphicsQueueFamilyIndex = UINT32_MAX;
	presentQueueFamilyIndex = UINT32_MAX;
	for (uint32_t i = 0; i < queueFamilyCount; ++i) {
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			if (graphicsQueueFamilyIndex == UINT32_MAX) graphicsQueueFamilyIndex = i;

			if (pSupportsPresent[i] == VK_TRUE) {
				graphicsQueueFamilyIndex = i;
				presentQueueFamilyIndex = i;
				break;
			}
		}
	}

	if (presentQueueFamilyIndex == UINT32_MAX) {
		// If didn't find a queue that supports both graphics and present, then
		// find a separate present queue.
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
			if (pSupportsPresent[i] == VK_TRUE) {
				presentQueueFamilyIndex = i;
				break;
			}
	}
	free(pSupportsPresent);

	// Generate error if could not find queues that support graphics
	// and present
	if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) {
		std::cout << "Could not find a queues for graphics and "
			"present\n";
		exit(-1);
	}

	// Get the list of VkFormats that are supported:
	uint32_t formatCount;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &formatCount, NULL);
	assert(result == VK_SUCCESS);
	VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &formatCount, surfFormats);
	assert(result == VK_SUCCESS);
	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
		format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else {
		assert(formatCount >= 1);
		format = surfFormats[0].format;
	}
	free(surfFormats);
}

void GraphicsSystem::InitializeSwapchain()
{
	VkResult result;

	VkSurfaceCapabilitiesKHR surfCapabilities;

	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfCapabilities);
	assert(result == VK_SUCCESS);

	uint32_t presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &presentModeCount, NULL);
	assert(result == VK_SUCCESS);
	VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &presentModeCount, presentModes);
	assert(result == VK_SUCCESS);

	VkExtent2D swapchainExtent;
	// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
	if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		swapchainExtent.width = (uint32_t)windowSize.x;
		swapchainExtent.height = (uint32_t)windowSize.y;
		if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
			swapchainExtent.width = surfCapabilities.minImageExtent.width;
		}
		else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
			swapchainExtent.width = surfCapabilities.maxImageExtent.width;
		}

		if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
			swapchainExtent.height = surfCapabilities.minImageExtent.height;
		}
		else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
			swapchainExtent.height = surfCapabilities.maxImageExtent.height;
		}
	}
	else {
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfCapabilities.currentExtent;
	}



	// The FIFO present mode is guaranteed by the spec to be supported
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	// Determine the number of VkImage's to use in the swap chain.
	// We need to acquire only 1 presentable image at at time.
	// Asking for minImageCount images ensures that we can acquire
	// 1 presentable image as long as we present it before attempting
	// to acquire another.
	uint32_t desiredNumberOfSwapChainImages = surfCapabilities.minImageCount;

	VkSurfaceTransformFlagBitsKHR preTransform;
	if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		preTransform = surfCapabilities.currentTransform;
	}

	// Find a supported composite alpha mode - one of these is guaranteed to be set
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
		if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
			compositeAlpha = compositeAlphaFlags[i];
			break;
		}
	}

	VkSwapchainCreateInfoKHR swapchain_ci = {};
	swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_ci.pNext = NULL;
	swapchain_ci.flags = 0;
	swapchain_ci.surface = surface;
	swapchain_ci.minImageCount = desiredNumberOfSwapChainImages;
	swapchain_ci.imageFormat = format;
	swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	swapchain_ci.imageExtent.width = swapchainExtent.width;
	swapchain_ci.imageExtent.height = swapchainExtent.height;
	swapchain_ci.imageArrayLayers = 1;
	swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_ci.queueFamilyIndexCount = 0;
	swapchain_ci.pQueueFamilyIndices = NULL;
	swapchain_ci.preTransform = preTransform;
	swapchain_ci.compositeAlpha = compositeAlpha;
	swapchain_ci.presentMode = swapchainPresentMode;
	swapchain_ci.clipped = true;
	swapchain_ci.oldSwapchain = VK_NULL_HANDLE;

	uint32_t queueFamilyIndices[2] = { (uint32_t)graphicsQueueFamilyIndex, (uint32_t)presentQueueFamilyIndex };
	if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
		// If the graphics and present queues are from different queue families,
		// we either have to explicitly transfer ownership of images between
		// the queues, or we have to create the swapchain with imageSharingMode
		// as VK_SHARING_MODE_CONCURRENT
		swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_ci.queueFamilyIndexCount = 2;
		swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
	}

	result = vkCreateSwapchainKHR(logicalDevice, &swapchain_ci, NULL, &swapchain);
	assert(result == VK_SUCCESS);

	result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount, NULL);
	assert(result == VK_SUCCESS);

	VkImage *swapchainImages = (VkImage *)malloc(swapchainImageCount * sizeof(VkImage));
	assert(swapchainImages);
	result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount, swapchainImages);
	assert(result == VK_SUCCESS);


	swapchainBuffers.resize(swapchainImageCount);
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		swapchainBuffers[i].image = swapchainImages[i];

		VkImageViewCreateInfo color_image_view = {};
		color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		color_image_view.pNext = NULL;
		color_image_view.flags = 0;
		color_image_view.image = swapchainBuffers[i].image;
		color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		color_image_view.format = format;
		color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
		color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
		color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
		color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
		color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		color_image_view.subresourceRange.baseMipLevel = 0;
		color_image_view.subresourceRange.levelCount = 1;
		color_image_view.subresourceRange.baseArrayLayer = 0;
		color_image_view.subresourceRange.layerCount = 1;

		result = vkCreateImageView(logicalDevice, &color_image_view, NULL, &swapchainBuffers[i].view);
		assert(result == VK_SUCCESS);
	}

	vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &graphicsQueue);
	if (graphicsQueueFamilyIndex == presentQueueFamilyIndex)
	{
		presentQueue = graphicsQueue;
	}
	else
	{
		vkGetDeviceQueue(logicalDevice, presentQueueFamilyIndex, 0, &presentQueue);
	}

	current_buffer = 0;
	free(presentModes);
	free(swapchainImages);
}

void GraphicsSystem::InitializeDepthBuffer()
{
	VkResult result;
	bool pass = false;

	VkImageCreateInfo image_info = {};

	const VkFormat depth_format = depth.format = VK_FORMAT_D16_UNORM;

	VkFormatProperties props;
	vkGetPhysicalDeviceFormatProperties(physicalDevices[0], depth_format, &props);
	if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
		image_info.tiling = VK_IMAGE_TILING_LINEAR;
	}
	else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	}
	else {
		/* Try other depth formats? */
		std::cout << "VK_FORMAT_D16_UNORM Unsupported.\n";
		exit(-1);
	}

	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.pNext = NULL;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.format = depth_format;
	image_info.extent.width = (uint32_t)windowSize.x;
	image_info.extent.height = (uint32_t)windowSize.y;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.samples = NUM_SAMPLES;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.queueFamilyIndexCount = 0;
	image_info.pQueueFamilyIndices = NULL;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	image_info.flags = 0;

	VkMemoryAllocateInfo mem_alloc = {};
	mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc.pNext = NULL;
	mem_alloc.allocationSize = 0;
	mem_alloc.memoryTypeIndex = 0;

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.pNext = NULL;
	view_info.image = VK_NULL_HANDLE;
	view_info.format = depth_format;
	view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.flags = 0;

	if (depth_format == VK_FORMAT_D16_UNORM_S8_UINT || depth_format == VK_FORMAT_D24_UNORM_S8_UINT ||
		depth_format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
		view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VkMemoryRequirements mem_reqs;

	/* Create image */
	result = vkCreateImage(logicalDevice, &image_info, NULL, &depth.image);
	assert(result == VK_SUCCESS);

	vkGetImageMemoryRequirements(logicalDevice, depth.image, &mem_reqs);

	mem_alloc.allocationSize = mem_reqs.size;
	/* Use the memory properties to determine the type of memory required */
	pass = memory_type_from_properties(memoryProperties, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
	assert(pass);

	/* Allocate memory */
	result = vkAllocateMemory(logicalDevice, &mem_alloc, NULL, &depth.mem);
	assert(result == VK_SUCCESS);

	/* Bind memory */
	result = vkBindImageMemory(logicalDevice, depth.image, depth.mem, 0);
	assert(result == VK_SUCCESS);

	/* Create image view */
	view_info.image = depth.image;
	result = vkCreateImageView(logicalDevice, &view_info, NULL, &depth.view);
	assert(result == VK_SUCCESS);
}

void GraphicsSystem::InitializeDescriptorPool()
{
	VkResult res;

	VkDescriptorPoolSize type_count[1];
	type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	type_count[0].descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptor_pool = {};
	descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool.pNext = NULL;
	descriptor_pool.maxSets = 1;
	descriptor_pool.poolSizeCount = 1;
	descriptor_pool.pPoolSizes = type_count;

	res = vkCreateDescriptorPool(logicalDevice, &descriptor_pool, NULL, &descriptorPool);
	assert(res == VK_SUCCESS);
}

void GraphicsSystem::InitializePipelineCache()
{
	VkResult res;

	VkPipelineCacheCreateInfo pipelineCache_ci;
	pipelineCache_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCache_ci.pNext = NULL;
	pipelineCache_ci.initialDataSize = 0;
	pipelineCache_ci.pInitialData = NULL;
	pipelineCache_ci.flags = 0;
	res = vkCreatePipelineCache(logicalDevice, &pipelineCache_ci, NULL, &pipelineCache);
	assert(res == VK_SUCCESS);

}

void GraphicsSystem::InitializePresentationSemaphores()
{
	VkResult res;

	VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
	imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageAcquiredSemaphoreCreateInfo.pNext = NULL;
	imageAcquiredSemaphoreCreateInfo.flags = 0;

	res = vkCreateSemaphore(logicalDevice, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
	assert(res == VK_SUCCESS);


	VkSemaphoreCreateInfo signalSemaphoreCreateInfo;
	signalSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	signalSemaphoreCreateInfo.pNext = NULL;
	signalSemaphoreCreateInfo.flags = 0;

	res = vkCreateSemaphore(logicalDevice, &signalSemaphoreCreateInfo, NULL, &signalPresentationSemaphore);
}

//Utility functions

bool GraphicsSystem::memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

void GraphicsSystem::InitViewports()
{
	viewport.height = (float)windowSize.x;
	viewport.width = (float)windowSize.y;
	viewport.minDepth = (float)0.0f;
	viewport.maxDepth = (float)1.0f;
	viewport.x = 0;
	viewport.y = 0;
	vkCmdSetViewport(commandBuffer,0, NUM_VIEWPORTS, &viewport);
}

void GraphicsSystem::InitScissors()
{
	scissor.extent.width = (uint32_t)windowSize.x;
	scissor.extent.height = (uint32_t)windowSize.y;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(commandBuffer, 0, NUM_SCISSORS, &scissor);
}

bool GraphicsSystem::PrepFrame()
{
	VkResult res;

	VkClearValue clear_values[2];
	clear_values[0].color.float32[0] = 0.2f;
	clear_values[0].color.float32[1] = 0.2f;
	clear_values[0].color.float32[2] = 0.2f;
	clear_values[0].color.float32[3] = 0.2f;
	clear_values[1].depthStencil.depth = 1.0f;
	clear_values[1].depthStencil.stencil = 0;


	VkCommandBufferBeginInfo cmd_buf_info = {};
	cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmd_buf_info.pNext = NULL;
	cmd_buf_info.flags = 0;
	cmd_buf_info.pInheritanceInfo = NULL;

	res = vkBeginCommandBuffer(commandBuffer, &cmd_buf_info);
	assert(res == VK_SUCCESS);

	InitViewports();
	InitScissors();

	return true;
}

bool GraphicsSystem::EndFrame()
{
	vkEndCommandBuffer(commandBuffer);

	const VkCommandBuffer cmd_bufs[] = { commandBuffer };
	
	//Get the index of the next available swapchain image:
	vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX /*TIMEOUT_WAIT*/, imageAcquiredSemaphore, VK_NULL_HANDLE,
		&current_buffer);

	VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit_info[1] = {};
	submit_info[0].pNext = NULL;
	submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info[0].waitSemaphoreCount = 1;
	submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;
	submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
	submit_info[0].commandBufferCount = 1;
	submit_info[0].pCommandBuffers = cmd_bufs;
	submit_info[0].signalSemaphoreCount = 1;
	submit_info[0].pSignalSemaphores = &signalPresentationSemaphore;

	/* Queue the command buffer for execution */
	vkQueueSubmit(graphicsQueue, 1, submit_info, NULL);

	/* Now present the image in the window */

	VkPresentInfoKHR present;
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.pNext = NULL;
	present.swapchainCount = 1;
	present.pSwapchains = &swapchain;
	present.pImageIndices = &current_buffer;
	present.waitSemaphoreCount = 1;
	present.pWaitSemaphores = &signalPresentationSemaphore;
	present.pResults = NULL;

	vkQueuePresentKHR(presentQueue, &present);

	vkDeviceWaitIdle(logicalDevice);

	//REMOVE THIS LINE after you've got the cube showing up again.
	vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX /*TIMEOUT_WAIT*/, imageAcquiredSemaphore, VK_NULL_HANDLE, &current_buffer);

	vkDestroySemaphore(logicalDevice, imageAcquiredSemaphore, NULL);

	vkDestroySemaphore(logicalDevice, signalPresentationSemaphore, NULL);

	return true;
}