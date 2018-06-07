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
		float rightMovement = -moveSpeed * input->GetKeyboard()->IsKeyDown('d');
		float leftMovement = moveSpeed * input->GetKeyboard()->IsKeyDown('a');

		float upMovement = moveSpeed * input->GetKeyboard()->IsKeyDown(VK_SPACE);
		float downMovement = -moveSpeed * input->GetKeyboard()->IsKeyDown(VK_LSHIFT);

		float forwardMovement = moveSpeed * input->GetKeyboard()->IsKeyDown('w');
		float backwardMovement = -moveSpeed * input->GetKeyboard()->IsKeyDown('s');

		glm::vec3 movement = { rightMovement + leftMovement, upMovement + downMovement, forwardMovement + backwardMovement };
		
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

	float moveSpeed = 0.15f;
};