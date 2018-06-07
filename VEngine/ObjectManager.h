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


	std::vector<GameObject*>* GetActiveObjects() { return &objects; }

private:
	std::vector<GameObject*> objects;

private:
	static ObjectManager* singleton;

public:
	static ObjectManager* GetManager() { return singleton; }

};

