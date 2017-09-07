#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "GameObject.h"

class RotateScript : public Component
{
private:
	glm::mat4x4 Projection;
	glm::mat4x4 View;
	glm::mat4x4 Clip;// Vulkan clip space has inverted Y and half Z.
	glm::mat4x4 Model;

	glm::mat4x4 MVP ;

public:
	RotateScript()
	{
		Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

		View = glm::lookAt(glm::vec3(-3, 3, -8),  // Camera is at (-5,3,-10), in World Space
			glm::vec3(0, 0, 0),     // and looks at the origin
			glm::vec3(0, 1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
		);

		Model = glm::mat4(1.0f);

		// Vulkan clip space has inverted Y and half Z.
		Clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.0f, 0.0f, 0.5f, 1.0f);

		MVP = Clip * Projection * View * Model;
	}

	~RotateScript()
	{
	}

	void Start()
	{

	}

	void Update()
	{
		owner->GetTransform()->rotate(0.4f);
	}
};