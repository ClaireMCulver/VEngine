#include "Camera.h"

Camera* Camera::mainCamera = NULL;

Camera::Camera(RenderPass * renderPass)
{
	if (mainCamera == NULL)
	{
		mainCamera = this;
	}

	mainRenderPass = renderPass;
	finalRenderOutput = mainRenderPass->GetImage(0);
}

void Camera::Render()
{
	UpdateMatrices();

	mainRenderPass->RecordBuffer(view, projection, VPMatrix, ObjectManager::GetManager()->GetRenderObjects());
	mainRenderPass->SubmitBuffer();
	mainRenderPass->ResetBuffer();

	for (size_t i = 0, count = imageEffects.size(); i < count; i++)
	{
		imageEffects[i]->RenderEffect();
	}
}
