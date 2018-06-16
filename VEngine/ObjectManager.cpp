#include "ObjectManager.h"

ObjectManager* ObjectManager::singleton = nullptr;

ObjectManager::ObjectManager()
{
	assert(singleton == nullptr);
	singleton = this;
}


ObjectManager::~ObjectManager()
{
}

void ObjectManager::AddObject(GameObject* object)
{
	activeObjects.push_back(object);
}

void ObjectManager::AddRenderObject(GameObject * renderObject)
{
	renderObjects.push_back(renderObject);
}

void ObjectManager::UpdateObjects()
{
	for (int i = 0, count = (int)activeObjects.size(); i < count; i++)
	{
		activeObjects[i]->Update();
	}
}

void ObjectManager::RenderObjects()
{

}

void ObjectManager::SetObjectActive(GameObject * object)
{
}

void ObjectManager::SetObjectInactive(GameObject * object)
{
}

