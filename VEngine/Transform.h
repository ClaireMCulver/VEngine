#pragma once
#include "GameObject.h"

#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtc\matrix_transform.hpp"

class Transform : public Component
{
private:
	// Local data //
	glm::vec3 position; //TODO: Node hierarchy, parents, local and world transforms. blech.
	glm::tquat<float> localRotation = { 0, 0, 0, 1 };

	// Matrix transformations //
	glm::mat4 modelMatrix;

public:
	Transform()
	{
	}

	~Transform()
	{
	}

	void Start() {}

	void Update() 
	{
		glm::mat4x4 rotationMatrix(localRotation);
		modelMatrix = glm::mat4x4(glm::translate(rotationMatrix, position));
	}

	// Get/Set //
	glm::vec3 GetPosition() { return position; }
	void SetPosition(glm::vec3 newPosition) { position = newPosition; }

	glm::mat4 GetModelMat() { return modelMatrix; }

	// Movement //
	void Translate(glm::vec3 translation) { position += translation; }

	//TODO: figure out whether the degrees passed should be changed to radians or not.
	void rotate(float degrees, glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f)) { localRotation *= glm::angleAxis(glm::radians(degrees), axis); }
};

