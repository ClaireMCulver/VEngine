#pragma once
#include "GameObject.h"

#include "glm\glm.hpp"

class Transform : public Component
{
private:
	glm::vec3 localPosition;
	glm::vec4 localRotation;

public:
	Transform()
	{
	}

	~Transform()
	{
	}

};

