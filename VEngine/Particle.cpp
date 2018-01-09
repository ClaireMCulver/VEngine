#pragma once

#include "GPUBuffer.h"
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"

struct ParticleInstanceData
{
	glm::vec3 position;
	glm::quat rotation = { 0, 0, 0, 1 };
};

class Particle
{
public:
	Particle()
	{

	}

	~Particle()
	{

	}

	void Start()
	{
		currentLife = lifeTime;
		alive = true;

		instanceData.position = { (float)(rand() % 30) / 10.0f - 0.5f, 0, (float)(rand() % 30) / 10.0f - 0.5f };
		velocity = { (float)(rand() % 10) / 10.0f - 0.5f, 1, (float)(rand() % 10) / 10.0f - 0.5f };
	}

	void Update(float deltaTime)
	{
		currentLife -= deltaTime;
		alive = currentLife <= 0.0f ? false : true;

		instanceData.position += velocity * deltaTime;
	}

	inline bool IsAlive()
	{
		return alive;
	}

	inline void Kill()
	{
		alive = false;
	}

	//Render specific data
	ParticleInstanceData instanceData;

	//Simulation data
	bool alive = false;
private:

	float lifeTime = 30.0f;
	float currentLife;

	glm::vec3 velocity;

};