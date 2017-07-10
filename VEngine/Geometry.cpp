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
	// Store the vertex buffer device side //
	vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, geometry.data(), sizeof(Triangle) * geometry.size());


}

bool Geometry::Draw(VkCommandBuffer commandBuffer)
{
	//TODO: Implement
}