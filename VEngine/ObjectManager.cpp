#include "ObjectManager.h"



ObjectManager::ObjectManager()
{
}


ObjectManager::~ObjectManager()
{
}

void ObjectManager::AddObject(GameObject* object)
{
	objects.push_back(object);
}

void ObjectManager::UpdateObjects()
{
	for (int i = 0, count = objects.size(); i < count; i++)
	{
		objects[i]->Update();
	}
}
