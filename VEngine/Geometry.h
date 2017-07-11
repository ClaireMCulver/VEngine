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

struct Triangle
{
	glm::vec3 vertexA;
	glm::vec3 vertexB;
	glm::vec3 vertexC;
	
	//Removed until I get file loading in. Either one works.
	//glm::vec3 normalA;
	//glm::vec3 normalB;
	//glm::vec3 normalC;

	glm::vec2 UVA;
	glm::vec2 UVB;
	glm::vec2 UVC;
};


class Geometry
{
public:
	Geometry();
	~Geometry();

private: 
	GPUBuffer* vertexBuffer;

	uint32_t numVertices;

public: 
	bool LoadMeshFromFile(char* filePath);
	bool LoadMesh(std::vector<Triangle> &geometry);

	bool Draw(VkCommandBuffer commandBuffer);
};

