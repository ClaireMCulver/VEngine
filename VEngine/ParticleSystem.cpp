#pragma once

#include "GameObject.h"
#include "GPUBuffer.h"
#include "Clock.h"

#include "Particle.cpp"

#include <list>

class ParticleSystem : public Component
{
public:

	ParticleSystem(int NumParticles)
	{
		numParticles = NumParticles;
		particles = new std::vector<Particle>(numParticles);
		particleSystemData = new std::vector<ParticleInstanceData>(numParticles);
	}

	~ParticleSystem()
	{
		std::cout << "This is fucking weird\n";
		//delete particles;
		//delete particleSystemData;
		//delete vertexBuffer;
		//delete instanceBuffer;
		std::cout << "I know right?\n";
	}

	void Start()
	{
		clockPtr = Clock::singleton;

		currentTime = timeUntilNextSpawn;

		std::vector<glm::vec3> particleVerts(6);
		particleVerts[0] = { 1.0f, -1.0f,  0.0f };
		particleVerts[2] = { -1.0f,  1.0f,  0.0f };
		particleVerts[1] = { -1.0f, -1.0f,  0.0f };
		particleVerts[3] = { 1.0f, -1.0f,  0.0f };
		particleVerts[4] = { 1.0f,  1.0f,  0.0f };
		particleVerts[5] = { -1.0f,  1.0f,  0.0f };

		std::vector<glm::vec3> particleNorms(6);
		particleNorms[0] = { 0.0f, 0.0f, -1.0f };
		particleNorms[1] = { 0.0f, 0.0f, -1.0f };
		particleNorms[2] = { 0.0f, 0.0f, -1.0f };
		particleNorms[3] = { 0.0f, 0.0f, -1.0f };
		particleNorms[4] = { 0.0f, 0.0f, -1.0f };
		particleNorms[5] = { 0.0f, 0.0f, -1.0f };

		std::vector<glm::vec2> particlUVs(6);
		particlUVs[0] = { 1.0f, 0.0f };
		particlUVs[2] = { 0.0f, 1.0f };
		particlUVs[1] = { 0.0f, 0.0f };
		particlUVs[3] = { 1.0f, 0.0f };
		particlUVs[4] = { 1.0f, 1.0f };
		particlUVs[5] = { 0.0f, 1.0f };

		std::vector<float> vertBuffer((particleVerts.size() * 3) + (particleNorms.size() * 3) + (particlUVs.size() * 2));
		for (int i = 0, k = 0, count = vertBuffer.size(); i < count;)
		{
			vertBuffer[i + 0] = particleVerts[k].x;
			vertBuffer[i + 1] = particleVerts[k].y;
			vertBuffer[i + 2] = particleVerts[k].z;

			vertBuffer[i + 3] = particleNorms[k].x;
			vertBuffer[i + 4] = particleNorms[k].y;
			vertBuffer[i + 5] = particleNorms[k].z;

			vertBuffer[i + 6] = particlUVs[k].x;
			vertBuffer[i + 7] = particlUVs[k].y;
			i += 8;
			k += 1;
		}

		vertexBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertBuffer.data(), sizeof(float) * vertBuffer.size());
		instanceBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(ParticleInstanceData) * numParticles);
	}

	void Update()
	{
		// Spawn particles
		currentTime -= static_cast<float>(clockPtr->DeltaTime());
		if (currentTime <= 0.f)
		{
			//Spawn new particle
			particleSpawnIndex = particleSpawnIndex % numParticles;
			enabledParticles.push_back(&(*particles)[particleSpawnIndex]); //Overrites a particle back at the start if all are active.
			enabledParticles.back()->Start();
			particleSpawnIndex++;

			if ((int)enabledParticles.size() > numParticles)
			{
				enabledParticles.pop_front();
			}

			currentTime = timeUntilNextSpawn;
		}


		for (std::list<Particle*>::const_iterator index = enabledParticles.begin(), end = enabledParticles.end(); index != end; )
		{
			if ((*index)->IsAlive())
			{//Update alive particles
				index++;
			}
			else
			{//Delete dead particles
				index = enabledParticles.erase(index); //Erase returns the correct new current index, so index is set to that.
			}

		}

		int i = 0;
		for (std::list<Particle*>::const_iterator index = enabledParticles.begin(), end = enabledParticles.end(); index != end; )
		{
			//Update enabled particles
			(*index)->Update(static_cast<float>(clockPtr->DeltaTime()));
			(*particleSystemData)[i] = (*index)->instanceData;
			i++;
			index++;
		}

		instanceBuffer->CopyMemoryIntoBuffer((*particleSystemData).data(), sizeof(ParticleInstanceData) * enabledParticles.size());
	}

	GPUBuffer* vertexBuffer;
	GPUBuffer* instanceBuffer;

public:
	inline int NumActiveParticles() { return enabledParticles.size(); }
	inline int NumParticles() { return numParticles; }

private:
	//Particle spawning;
	Clock* clockPtr = nullptr;
	float timeUntilNextSpawn = 0.1f;
	float currentTime = 0.5f;

	std::vector<Particle> *particles;
	std::vector<ParticleInstanceData> *particleSystemData;

	std::list<Particle*> enabledParticles;
	int numParticles;

	int particleSpawnIndex = 0;
};