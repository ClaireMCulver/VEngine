#pragma once

#include "Renderer.cpp"

class MeshRenderer : public Renderer
{
public:
	MeshRenderer()
	{

	}

	~MeshRenderer()
	{
	}

	void Start()
	{
		objectGeometry = owner->GetGeometry();
		assert(objectGeometry != nullptr);

		owner->SetRenderer(this);
	}

	void Update()
	{

	}

	void Draw(VkCommandBuffer commandBuffer)
	{
		RenderMesh(commandBuffer, objectGeometry->GetNumVertices(), objectGeometry->GetVertexBuffer()->GetVKBuffer(), owner->GetInstanceBuffer()->GetVKBuffer(), 1);
	}

private:
	Geometry* objectGeometry;
};