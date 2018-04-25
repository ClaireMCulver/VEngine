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
		input = Input::singleton;
		camera = owner->GetComponent<Camera>();
	}

	void Update()
	{
		float rightMovement = moveSpeed * input->GetKeyboard()->IsKeyDown('w');
		float leftMovement = -moveSpeed * input->GetKeyboard()->IsKeyDown('s');

		float upMovement = moveSpeed * input->GetKeyboard()->IsKeyDown('a');
		float downMovement = -moveSpeed * input->GetKeyboard()->IsKeyDown('d');

		float forwardMovement = -moveSpeed * input->GetKeyboard()->IsKeyDown(VK_LSHIFT);
		float backwardMovement = moveSpeed * input->GetKeyboard()->IsKeyDown(VK_SPACE);

		glm::vec3 movement = { upMovement + downMovement, forwardMovement + backwardMovement, rightMovement + leftMovement };

		float rotation = (float)input->GetKeyboard()->IsKeyDown('z');
		rotation += (float)input->GetKeyboard()->IsKeyDown('x');

		transform->Translate(movement);
		transform->rotate(rotation);

		camera->SetLookPoint(camera->GetViewPoint() + movement);
	}

private:
	Transform* transform;
	Input* input;
	Camera* camera;

	float moveSpeed = 0.05f;
};