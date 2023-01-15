#pragma once

#include "ImageEffect.cpp"
#include "Camera.h"
#include "MRTCompositeRenderPass.h"

class FXAAEffect : public ImageEffect
{
public:
	FXAAEffect() {}
	~FXAAEffect()
	{

	}

	void Start() 
	{
		effectRenderPass = new MRTCompositeRenderPass(WindowSize[0], WindowSize[1]);

		Shader textureMergeVertShader("../Assets/Shaders/ScreenEffectPassthrough.vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
		Shader textureMergeFragShader("../Assets/Shaders/fxaa.frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

		effectMaterial = new Material();
		effectMaterial->AddShader(textureMergeVertShader);
		effectMaterial->AddShader(textureMergeFragShader);
		effectMaterial->FinalizeMaterial(*effectRenderPass);

		SetSourceTexture(owner->GetComponent<Camera>()->GetPrimaryRenderOutput(0));
		owner->GetComponent<Camera>()->SetFinalOutput(effectRenderPass->GetImage(0));
		owner->GetComponent<Camera>()->AddImageEffect(this);
	}

	void Update() {}

	// Inherited via ImageEffect
	virtual void RenderEffect() override
	{
		Camera* camera = owner->GetComponent<Camera>();
		effectRenderPass->BeginPass(camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetVPMatrix());

		effectMaterial->BindMaterial(glm::mat4(1), glm::mat4(1), glm::mat4(1), effectRenderPass->GetRenderBuffer(), effectRenderPass->GetVKPipelineLayout());

		RenderQuad(effectRenderPass->GetRenderBuffer()->GetVKCommandBuffer());

		effectRenderPass->EndPass();

		effectRenderPass->SubmitBuffer();
		effectRenderPass->ResetBuffer();
	}
};