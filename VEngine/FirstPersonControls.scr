#pragma once

#include "GameObject.h"
#include "Input.h"
#include "Camera.h"

class FirstPersonControls : public Component
{
public:
	FirstPersonControls()
	{
	}

	~FirstPersonControls()
	{
	}

	void Start()
	{
		transform = owner->GetTransform();
		input = Input::Singleton;
		camera = owner->GetComponent<Camera>();
	}

	void Update()
	{
		float rightMovement = moveSpeed * input->keyboard.IsKeyDown('w');
		float leftMovement = -moveSpeed * input->keyboard.IsKeyDown('s');

		float upMovement = moveSpeed * input->keyboard.IsKeyDown('a');
		float downMovement = -moveSpeed * input->keyboard.IsKeyDown('d');

		float forwardMovement = -moveSpeed * input->keyboard.IsKeyDown(VK_LSHIFT);
		float backwardMovement = moveSpeed * input->keyboard.IsKeyDown(VK_SPACE);

		glm::vec3 movement = { upMovement + downMovement, forwardMovement + backwardMovement, rightMovement + leftMovement };

		transform->Translate(movement);
		camera->SetLookPoint(camera->GetViewPoint() + movement);
	}

private:
	Transform* transform;
	Input* input;
	Camera* camera;

	float moveSpeed = 0.05f;
};