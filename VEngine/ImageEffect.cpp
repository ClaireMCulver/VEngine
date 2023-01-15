#pragma once

#include "GraphicsDefs.h"
#include "GameObject.h"
#include "Renderer.cpp"

#include "RenderTexture.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "Material.h"

#include "Input.h"

class ImageEffect : public Component
{
public:
	ImageEffect() 
	{
		glm::vec3 lb = { -1, -1, 0 };
		glm::vec3 lt = { -1,  1, 0 };
		glm::vec3 rb = { 1, -1, 0 };
		glm::vec3 rt = { 1,  1, 0 };

		fsQuad.resize(2);

		fsQuad[0].vertexA = lb;
		fsQuad[0].vertexB = lt;
		fsQuad[0].vertexC = rt;
		fsQuad[1].vertexA = lb;
		fsQuad[1].vertexB = rt;
		fsQuad[1].vertexC = rb;

		vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, fsQuad.data(), sizeof(Triangle) * fsQuad.size());
		glm::mat4 indentityPtr = glm::mat4(1.0);
		instanceBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &indentityPtr, 64); //This isn't used, but it needs to be filled and passed, both because vulkan will complain because my standardized descriptor sets.	
	}

	~ImageEffect() 
	{
		delete inputImage;

		delete vertexBuffer;
		delete instanceBuffer;

		assert(effectMaterial);
		delete effectMaterial;

		assert(effectRenderPass);
		delete effectRenderPass;
	}
	
	virtual void RenderEffect() = 0;

private: 
	void Start() 
	{
	}

	void Update() 
	{
	}

protected:

	void SetRenderPass(RenderPass* renderPass) { effectRenderPass = renderPass; }

	void RenderQuad(VkCommandBuffer commandBuffer) 
	{
		VkBuffer vertBuffer = vertexBuffer->GetVKBuffer();
		VkBuffer instBuffer = instanceBuffer->GetVKBuffer();
		const VkDeviceSize offsets[1] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertBuffer, offsets);
		vkCmdBindVertexBuffers(commandBuffer, 1, 1, &instBuffer, offsets);

		vkCmdDraw(commandBuffer, 6, 1, 0, 0);
	}

	void SetSourceTexture(Image* source) 
	{
		inputImage = new RenderTexture(source);
		
		assert(effectMaterial);
		effectMaterial->SetTexture(*inputImage, 0);
		effectMaterial->SetTexture(*inputImage, 1);
		effectMaterial->SetTexture(*inputImage, 2);
		effectMaterial->SetTexture(*inputImage, 3);
		effectMaterial->SetTexture(*inputImage, 4);
		effectMaterial->SetTexture(*inputImage, 5);
	}

private:
	// Variables for presenting to the screen.
	RenderTexture* inputImage;
	Image* outputImage;

	std::vector<Triangle> fsQuad;

	GPUBuffer* vertexBuffer;
	GPUBuffer* instanceBuffer;

protected:
	RenderPass* effectRenderPass;
	Material* effectMaterial;
};