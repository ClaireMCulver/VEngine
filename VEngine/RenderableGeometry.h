#pragma once

#include <stdio.h>
#include <vector>

#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

#include "glm\vec3.hpp"


#include "GraphicsSystem.h"
#include "Material.h"

struct VertexBuffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDescriptorBufferInfo buffer_info;
};

class RenderableGeometry
{
public:
	RenderableGeometry();
	~RenderableGeometry();
	
	bool LoadMeshFromFile(char* filePath);
	bool LoadMesh(std::vector<Triangle> *geometry);

	bool Render(VkCommandBuffer commandBuffer);

public:
	Material objectMaterial;

	VkCommandBuffer renderCmdBuffer;
	bool InitializeCommandBuffer();
	void RecordCommandBuffer(VkRenderPass renderPass);
	void RenderObject(VkCommandBuffer &primaryBuffer);

private:

	bool FindMemoryType(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

private:
	VertexBuffer vertexBuffer;

};