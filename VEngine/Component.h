#pragma once

class GameObject;

class Component
{
public:
	Component() {};
	~Component() {};

	//Function is called when the component is added to the game object.
	virtual void Start() = 0;

	//Function is called once each frame
	virtual void Update() = 0;

	void SetOwner(GameObject* object) { owner = object; }

protected:
	GameObject* owner;
};