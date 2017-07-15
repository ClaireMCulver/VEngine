#pragma once

#include <assert.h>
#include <vector>

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include <glm\common.hpp>

#include "Graphics.h"
#include "GPUBuffer.h"
#include "GraphicsDefs.h"

class Geometry
{
public:
	Geometry();
	~Geometry();

private: 
	GPUBuffer* vertexBuffer;

	uint32_t numVertices;

public: 
	void LoadMeshFromFile(char* filePath);
	void LoadMesh(std::vector<Triangle> &geometry);
	
	void Draw(VkCommandBuffer commandBuffer);
};

