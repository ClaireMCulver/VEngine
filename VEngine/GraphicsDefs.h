#pragma once

#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"
#include <vector>

#include "glm\vec4.hpp"
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
	glm::vec4 vertexA;
	glm::vec2 uvA;
	glm::vec4 vertexB;
	glm::vec2 uvB;
	glm::vec4 vertexC;
	glm::vec2 uvC;

	//Removed until I get file loading in. Either one works.
	//glm::vec3 normalA;
	//glm::vec3 normalB;
	//glm::vec3 normalC;
};

#define XYZ1(_x_, _y_, _z_) glm::vec4(_x_, _y_, _z_, 1.f)
#define UV(_u_, _v_) glm::vec2(_u_, _v_)

static const Triangle g_vbData[] = {
	{ XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f) },{ XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f) },{ XYZ1(-1, 1, -1),  XYZ1(0.f, 1.f, 0.f) },
	{ XYZ1(-1, 1, -1),  XYZ1(0.f, 1.f, 0.f) },{ XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f) },{ XYZ1(1, 1, -1),   XYZ1(1.f, 1.f, 0.f) },
																										   
	{ XYZ1(-1, -1, 1),  XYZ1(0.f, 0.f, 1.f) },{ XYZ1(-1, 1, 1),  XYZ1(0.f, 1.f, 1.f) },{ XYZ1(1, -1, 1),   XYZ1(1.f, 0.f, 1.f) },
	{ XYZ1(1, -1, 1),   XYZ1(1.f, 0.f, 1.f) },{ XYZ1(-1, 1, 1),  XYZ1(0.f, 1.f, 1.f) },{ XYZ1(1, 1, 1),    XYZ1(1.f, 1.f, 1.f) },
					    									     										   
	{ XYZ1(1, 1, 1),    XYZ1(1.f, 1.f, 1.f) },{ XYZ1(1, 1, -1),  XYZ1(1.f, 1.f, 0.f) },{ XYZ1(1, -1, 1),   XYZ1(1.f, 0.f, 1.f) },
	{ XYZ1(1, -1, 1),   XYZ1(1.f, 0.f, 1.f) },{ XYZ1(1, 1, -1),  XYZ1(1.f, 1.f, 0.f) },{ XYZ1(1, -1, -1),  XYZ1(1.f, 0.f, 0.f) },
					    
	{ XYZ1(-1, 1, 1),   XYZ1(0.f, 1.f, 1.f) },{ XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f) },{ XYZ1(-1, 1, -1),  XYZ1(0.f, 1.f, 0.f) },
	{ XYZ1(-1, 1, -1),  XYZ1(0.f, 1.f, 0.f) },{ XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f) },{ XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f) },
					    
	{ XYZ1(1, 1, 1),    XYZ1(1.f, 1.f, 1.f) },{ XYZ1(-1, 1, 1),  XYZ1(0.f, 1.f, 1.f) },{ XYZ1(1, 1, -1),   XYZ1(1.f, 1.f, 0.f) },
	{ XYZ1(1, 1, -1),   XYZ1(1.f, 1.f, 0.f) },{ XYZ1(-1, 1, 1),  XYZ1(0.f, 1.f, 1.f) },{ XYZ1(-1, 1, -1),  XYZ1(0.f, 1.f, 0.f) },

	{ XYZ1(1, -1, 1),   XYZ1(1.f, 0.f, 1.f) },{ XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f) },{ XYZ1(-1, -1, 1),  XYZ1(0.f, 0.f, 1.f) },
	{ XYZ1(-1, -1, 1),  XYZ1(0.f, 0.f, 1.f) },{ XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f) },{ XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f) },
};

static const Triangle g_vb_solid_face_colors_Data[] = {
	// red face
	{ XYZ1(-1, -1, 1), XYZ1(1.f, 0.f, 0.f) },
	{ XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f) },
	{ XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f) },
	{ XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f) },
	{ XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f) },
	{ XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 0.f) },
	// green face
	{ XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 0.f) },
	{ XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f) },
	{ XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f) },
	{ XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f) },
	{ XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f) },
	{ XYZ1(1, 1, -1), XYZ1(0.f, 1.f, 0.f) },
	// blue face
	{ XYZ1(-1, 1, 1), XYZ1(0.f, 0.f, 1.f) },
	{ XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f) },
	{ XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f) },
	{ XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f) },
	{ XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f) },
	{ XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 1.f) },
	// yellow face
	{ XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 0.f) },
	{ XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f) },
	{ XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f) },
	{ XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f) },
	{ XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f) },
	{ XYZ1(1, -1, -1), XYZ1(1.f, 1.f, 0.f) },
	// magenta face
	{ XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 1.f) },
	{ XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f) },
	{ XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f) },
	{ XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f) },
	{ XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f) },
	{ XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 1.f) },
	// cyan face
	{ XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 1.f) },
	{ XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f) },
	{ XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f) },
	{ XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f) },
	{ XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f) },
	{ XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 1.f) },
};

static std::vector<Triangle> g_vb_texture_Data = {
	// left face
	{ XYZ1(-1, -1, -1), UV(1.f, 0.f) ,  // lft-top-front
	  XYZ1(-1, 1, 1),   UV(0.f, 1.f) ,    // lft-btm-back
	  XYZ1(-1, -1, 1),  UV(0.f, 0.f) },   // lft-top-back

	{ XYZ1(-1, 1, 1),   UV(0.f, 1.f) ,    // lft-btm-back
	  XYZ1(-1, -1, -1), UV(1.f, 0.f) ,  // lft-top-front
	  XYZ1(-1, 1, -1),  UV(1.f, 1.f) },   // lft-btm-front

	// front face
	{ XYZ1(-1, -1, -1), UV(0.f, 0.f) ,  // lft-top-front
	  XYZ1(1, -1, -1),  UV(1.f, 0.f) ,   // rgt-top-front
	  XYZ1(1, 1, -1),   UV(1.f, 1.f) }, // rgt-btm-front

	{ XYZ1(-1, -1, -1), UV(0.f, 0.f) ,  // lft-top-front
	  XYZ1(1, 1, -1),   UV(1.f, 1.f) ,    // rgt-btm-front
	  XYZ1(-1, 1, -1),  UV(0.f, 1.f) },   // lft-btm-front

	// top face
	{ XYZ1(-1, -1, -1), UV(0.f, 1.f) ,  // lft-top-front
	  XYZ1(1, -1, 1),   UV(1.f, 0.f)  ,    // rgt-top-back
	  XYZ1(1, -1, -1),  UV(1.f, 1.f) },   // rgt-top-front

	{ XYZ1(-1, -1, -1), UV(0.f, 1.f),  // lft-top-front
	  XYZ1(-1, -1, 1),  UV(0.f, 0.f),   // lft-top-back
	  XYZ1(1, -1, 1),   UV(1.f, 0.f) },    // rgt-top-back
	
	// bottom face
	{ XYZ1(-1, 1, -1),  UV(0.f, 0.f),  // lft-btm-front
	  XYZ1(1, 1, 1),    UV(1.f, 1.f),    // rgt-btm-back
	  XYZ1(-1, 1, 1),   UV(0.f, 1.f) },   // lft-btm-back

	{ XYZ1(-1, 1, -1),  UV(0.f, 0.f),  // lft-btm-front
	  XYZ1(1, 1, -1),   UV(1.f, 0.f),   // rgt-btm-front
	  XYZ1(1, 1, 1),    UV(1.f, 1.f) },    // rgt-btm-back
					    
	// right face	    
	{ XYZ1(1, 1, -1),   UV(0.f, 1.f),   // rgt-btm-front
	  XYZ1(1, -1, 1),   UV(1.f, 0.f),   // rgt-top-back
	  XYZ1(1, 1, 1),    UV(1.f, 1.f) },    // rgt-btm-back
					    
	{ XYZ1(1, -1, 1),   UV(1.f, 0.f),   // rgt-top-back
	  XYZ1(1, 1, -1),   UV(0.f, 1.f),   // rgt-btm-front
	  XYZ1(1, -1, -1),  UV(0.f, 0.f) },  // rgt-top-front

	// back face
	{ XYZ1(-1, 1, 1),   UV(1.f, 1.f),   // lft-btm-back
	  XYZ1(1, 1, 1),    UV(0.f, 1.f),    // rgt-btm-back
	  XYZ1(-1, -1, 1),  UV(1.f, 0.f) },  // lft-top-back
					    
	{ XYZ1(-1, -1, 1),  UV(1.f, 0.f),  // lft-top-back
	  XYZ1(1, 1, 1),    UV(0.f, 1.f),    // rgt-btm-back
	  XYZ1(1, -1, 1),   UV(0.f, 0.f) },   // rgt-top-back
};