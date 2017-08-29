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
	glm::tquat<float> localRotation;

	// Matrix transformations //
	glm::mat4 modelMatrix;

public:
	Transform()
	{
	}

	~Transform()
	{
	}

	// Get/Set //
	glm::vec3 GetPosition() { return position; }

	inline void UpdateModelMatrix() { modelMatrix; }

	void Translate(glm::vec3 translation) { position += translation; }

	glm::mat4 GetModelMat() { return modelMatrix; }
};

