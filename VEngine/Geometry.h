#pragma once

#include <assert.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include <glm\common.hpp>

#include "rapidxml\rapidxml.hpp"
#include "rapidxml\rapidxml_utils.hpp"

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
	//GPUBuffer* indexBuffer; //Not doing index buffers yet. It's stupid.

	uint32_t numVertices;

public: 
	void LoadMeshFromDae(char* filePath);
	
	void Draw(VkCommandBuffer commandBuffer);

private:
	void LoadMesh(std::vector<Triangle> &geometry, std::vector<uint32_t> &indices);
	std::vector<glm::vec3> loadVertices(rapidxml::xml_node<> *sourceNode);
	std::vector<glm::vec2> loadUVs(rapidxml::xml_node<> *sourceNode);
};

