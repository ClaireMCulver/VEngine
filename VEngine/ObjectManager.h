#pragma once

#include <vector>
#include <assert.h>

#include "GameObject.h"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void AddObject(GameObject* object);
	void AddRenderObject(GameObject* renderObject);
	void UpdateObjects();
	void RenderObjects();

	void SetObjectActive(GameObject* object);
	void SetObjectInactive(GameObject* object);

	std::vector<GameObject*>* GetActiveObjects() { return &activeObjects; }
	std::vector<GameObject*>* GetRenderObjects() { return &renderObjects; }

private:
	std::vector<GameObject*> activeObjects;
	std::vector<GameObject*> inactiveObjects;
	
	std::vector<GameObject*> renderObjects;

private:
	static ObjectManager* singleton;

public:
	static ObjectManager* GetManager() { return singleton; }

};

