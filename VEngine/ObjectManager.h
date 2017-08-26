#pragma once

#include <vector>

#include "GameObject.h"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void AddObject(GameObject* object);
	void UpdateObjects();

private:
	std::vector<GameObject*> objects;
};

