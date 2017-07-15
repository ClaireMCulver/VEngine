#include "Geometry.h"



Geometry::Geometry()
{
}


Geometry::~Geometry()
{
	if (vertexBuffer != NULL)
	{
		delete vertexBuffer;
	}
}

void Geometry::LoadMeshFromFile(char* filePath)
{
	//TODO: Implement file loading and parsing, then just pass the parsed file to the load mesh function.
}

void Geometry::LoadMesh(std::vector<Triangle> &geometry)
{
	// Store the number of vertices in the geometry //
	numVertices = geometry.size() * 3;

	// Store the vertex buffer device side //
	vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, geometry.data(), sizeof(Triangle) * geometry.size());
}

void Geometry::Draw(VkCommandBuffer commandBuffer)
{
	const VkBuffer vkBuffer = vertexBuffer->GetVKBuffer();
	const VkDeviceSize offsets[1] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vkBuffer, offsets);

	vkCmdDraw(commandBuffer, numVertices, 1, 0, 0);
}