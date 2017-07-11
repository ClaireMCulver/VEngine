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

bool Geometry::LoadMeshFromFile(char* filePath)
{
	//TODO: Implement file loading and parsing, then just pass the parsed file to the load mesh function.
}

bool Geometry::LoadMesh(std::vector<Triangle> &geometry)
{
	// Store the number of vertices in the geometry //
	numVertices = geometry.size() * 3;

	// Store the vertex buffer device side //
	vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, geometry.data(), sizeof(Triangle) * geometry.size());


}

bool Geometry::Draw(VkCommandBuffer commandBuffer)
{
	//TODO: Implement
	const VkBuffer vkBuffer = vertexBuffer->GetVKBuffer();
	const VkDeviceSize offsets[1] = { 0 };

	vkCmdBindVertexBuffers(fuck, 0, 1, &vkBuffer, offsets); 

	vkCmdDraw(fuck, numVertices, 1, 0, 0);
}