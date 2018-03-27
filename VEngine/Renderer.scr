#pragma once

#include "Component.h"
#include "Geometry.h"
#include "GPUBuffer.h"


class Renderer : public Component
{
public:
	Renderer()
	{
	}

	~Renderer()
	{

	}

	void Start()
	{	
	}

	void Update()
	{

	}

	virtual void Draw(VkCommandBuffer commandBuffer) = 0;

	void RenderMesh(VkCommandBuffer commandBuffer, uint32_t vertexCount, VkBuffer vertexBuffer, VkBuffer InstanceBuffer, uint32_t NumInstances)
	{
		const VkDeviceSize offsets[1] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
		vkCmdBindVertexBuffers(commandBuffer, 1, 1, &InstanceBuffer, offsets);

		vkCmdDraw(commandBuffer, vertexCount, NumInstances, 0, 0);
	}

private:
};