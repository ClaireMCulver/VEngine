#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "GameObject.h"
#include "Component.h"
#include "RenderPass.h"
#include "Image.h"
#include "ImageEffect.cpp"

class Camera : public Component
{
private:

	glm::vec3 viewPoint = glm::vec3(0.f);
	glm::vec3 viewVector = glm::vec3(0.f);
	glm::vec3 UpVector = glm::vec3(0.f, 1.f, 0.f);

	//This definition exist to serve as a reminder of the lookat function and how the lookat works.
	glm::mat4x4 view = glm::lookAt(
		glm::vec3(1, 1, 1),		// Camera is at (1,1,1), in World Space
		glm::vec3(0, 0, 0),		// and looks at the origin
		glm::vec3(0, 1, 0)		// Head is up (set to 0,-1,0 to look upside-down)
	);

	//fov, aspect, near-plane, far-plane.
	glm::mat4x4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);

	//This is constant because clip space in vulkan is a constant of the api.
	const glm::mat4x4 clip = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f);// Vulkan clip space has inverted Y and half Z.

	glm::mat4x4 VPMatrix = clip * projection * view;

	static Camera* mainCamera;

	RenderPass* mainRenderPass;
	std::vector<ImageEffect*> imageEffects;

	Image* finalRenderOutput;

public:
	Camera(RenderPass* renderPass);


	~Camera()
	{
	}

	// Main Camera //
	static void SetMain(Camera* cameraPtr) { mainCamera = cameraPtr; }
	static Camera* GetMain() { return mainCamera; }

	// Get Matrices //
	inline glm::mat4x4 GetViewMatrix() { return view; }
	inline glm::mat4x4 GetProjectionMatrix() { return projection; }
	inline glm::mat4x4 GetClipMatrix() { return clip; }
	inline glm::mat4x4 GetVPMatrix() { return VPMatrix; }

	// Camera Matrices //
	void SetPerspective(float degrees, float aspect, float nearPlane, float farPlane) 
	{ 
		projection = glm::perspective(glm::radians(degrees), aspect, nearPlane, farPlane); 
	}

	//Tells the camera to look at the point provided.
	void LookAt(glm::vec3 point, glm::vec3 up = glm::vec3(0, 1, 0)) 
	{
		view = glm::lookAt
		(
			owner->GetTransform()->GetPosition(),
			point,
			up
		);

		viewVector = glm::normalize(point - owner->GetTransform()->GetPosition());
	}

	void UpdateMatrices()
	{
		view = glm::lookAt
		(
			owner->GetTransform()->GetPosition(),
			viewPoint,
			UpVector
		);

		VPMatrix = clip * projection * view;
	}

	// Get/Set //
	void SetLookPoint(glm::vec3 point) { viewPoint = point; viewVector = glm::normalize(point - owner->GetTransform()->GetPosition()); }
	glm::vec3 GetViewVector() { return viewVector; }
	glm::vec3 GetViewPoint() { return viewPoint; }

	// Image Effects //
	void SetFinalOutput(Image* outputImage) { finalRenderOutput = outputImage; }
	Image* GetFinalOutput() { return finalRenderOutput; }

	Image* GetPrimaryRenderOutput(int index) { return mainRenderPass->GetImage(index); }

	void AddImageEffect(ImageEffect* imageEffect) { imageEffects.push_back(imageEffect); }

	// Component Functions //
	void Start() {}
	void Update() {}

	void Render();

};