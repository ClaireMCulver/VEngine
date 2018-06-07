#pragma once

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"
#include <vector>

#include "glm\vec3.hpp"
#include "glm\vec2.hpp"

#define NUM_DESCRIPTOR_SETS 1
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT
#define FENCE_TIMEOUT 100000000
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS
#define TIMEOUT_WAIT 500

/*
* Keep each of our swap chain buffers' image, command buffer and view in one
* spot
*/
typedef struct swapchain_buffers {
	VkImage image;
	VkImageView view;
} SwapchainBuffer;

typedef struct depth_buffer {
	VkFormat format;

	VkImage image;
	VkDeviceMemory mem;
	VkImageView view;
} DepthBuffer;

typedef struct uniform_data {
	VkBuffer buf;
	VkDeviceMemory mem;
	VkDescriptorBufferInfo buffer_info;
} UniformData;

typedef struct LayerProperties {
	VkLayerProperties properties;
	std::vector<VkExtensionProperties> extensions;
} layer_properties;

struct Vertex {
	float posX, posY, posZ, posW;  // Position data
	float r, g, b, a;              // Color
};

struct VertexUV {
	float posX, posY, posZ, posW;  // Position data
	float u, v;                    // texture u,v
};

struct Triangle
{
	glm::vec3 vertexA; 
	glm::vec3 normalA;
	glm::vec2 uvA;
	glm::vec3 tangentA;
	glm::vec3 biTangentA;
	
	glm::vec3 vertexB;
	glm::vec3 normalB;
	glm::vec2 uvB;
	glm::vec3 tangentB;
	glm::vec3 biTangentB;

	glm::vec3 vertexC;
	glm::vec3 normalC;
	glm::vec2 uvC;
	glm::vec3 tangentC;
	glm::vec3 biTangentC;
};