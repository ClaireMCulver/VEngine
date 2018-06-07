#pragma once

#include "GameObject.h"
#include "Renderer.scr"

#include "RenderTexture.h"
#include "CommandBuffer.h"

#include "Input.h"

class FXAARenderer : public Renderer
{
public:
	FXAARenderer(glm::vec2 textureASize, glm::vec2 textureBSize) {
	}
	~FXAARenderer() {
		delete previousTexture;

		delete vertexBuffer;
		delete instanceBuffer;
	}

	void Start() {
		owner->SetRenderer(this);

		inputPtr = Input::singleton;

		glm::vec3 lb = { -1, -1, 0 };
		glm::vec3 lt = { -1,  1, 0 };
		glm::vec3 rb = { 1, -1, 0 };
		glm::vec3 rt = { 1,  1, 0 };
		int step = (3 + 3 + 2 + 3 + 3);

		fsQuad[0] = lb.x;
		fsQuad[1] = lb.y;

		fsQuad[0 + step] = lt.x;
		fsQuad[1 + step] = lt.y;

		fsQuad[0 + step * 2] = rb.x;
		fsQuad[1 + step * 2] = rb.y;

		fsQuad[0 + step * 3] = rt.x;
		fsQuad[1 + step * 3] = rt.y;

		vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, fsQuad, (sizeof(float)) * step * 4);
		glm::mat4 indentityPtr = glm::mat4(1.0);
		instanceBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &indentityPtr, 64); //This isn't used, but it needs to be filled and passed, both because vulkan will complain because my standardized descriptor sets.

		
	}


	void Update() {
	}

	void Draw(VkCommandBuffer commandBuffer) {
		RenderMesh(commandBuffer, 4, vertexBuffer->GetVKBuffer(), instanceBuffer->GetVKBuffer(), 1);
	}

	void SetSourceTexture(Image* source) {
		previousTexture = new RenderTexture(source);
		owner->SetTexture(*previousTexture, 0);
		owner->SetTexture(*previousTexture, 1);
		owner->SetTexture(*previousTexture, 2);
		owner->SetTexture(*previousTexture, 3);
		owner->SetTexture(*previousTexture, 4);
		owner->SetTexture(*previousTexture, 5);
	}


private:

private:
	// Variables for presenting to the screen.
	RenderTexture* previousTexture;

	float fsQuad[(3 + 3 + 2 + 3 + 3) * 4];

	GPUBuffer* vertexBuffer;
	GPUBuffer* instanceBuffer;

	Input* inputPtr;
};