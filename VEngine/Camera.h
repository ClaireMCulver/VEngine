#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "GameObject.h"
class Camera : public Component
{
private:
	
	//This definition exist to serve as a reminder of the lookat function and how the view matrix works.
	glm::mat4x4 View = glm::lookAt(
		glm::vec3(1, 1, 1),		// Camera is at (1,1,1), in World Space
		glm::vec3(0, 0, 0),		// and looks at the origin
		glm::vec3(0, 1, 0)		// Head is up (set to 0,-1,0 to look upside-down)
	);

	//fov, aspect, near-plane, far-plane.
	glm::mat4x4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

	//This is constant because clip space in vulkan is a constant of the api.
	const glm::mat4x4 Clip = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.0f, 0.0f, 0.5f, 1.0f);// Vulkan clip space has inverted Y and half Z.

	static Camera* mainCamera;


public:
	Camera()
	{
		if (mainCamera == NULL)
		{
			mainCamera = this;
		}
	}

	~Camera()
	{
	}

	
	// Main Camera //
	static void SetMain(Camera* cameraPtr) { mainCamera = cameraPtr; }
	static Camera* GetMain() { return mainCamera; }


	// Camera Matrices //
	void SetPerspective(float degrees, float aspect, float nearPlane, float farPlane) 
	{ 
		Projection = glm::perspective(glm::radians(degrees), aspect, nearPlane, farPlane); 

	}

	//Tells the camera to look at the point provided.
	void LookAt(glm::vec3 point, glm::vec3 up = glm::vec3(0, 1, 0)) 
	{
		View = glm::lookAt
		(
			owner->getTransform()->GetPosition(),
			point,
			up
		);
	}

};

Camera* Camera::mainCamera = NULL;