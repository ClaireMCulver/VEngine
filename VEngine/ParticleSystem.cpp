#pragma once

#include "GameObject.h"
#include "GPUBuffer.h"
#include "Clock.h"

#include "Particle.cpp"
#include "Camera.h"

#include <list>

#define comp2(a,b) (*( ParticleInstanceData* )a < *( ParticleInstanceData* )b)

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
		delete particles;
		delete particleSystemData;
		delete vertexBuffer;
		delete instanceBuffer;
	}

	void Start()
	{
		clockPtr = Clock::singleton;

		currentTime = timeUntilNextSpawn;

		std::vector<glm::vec3> particleVerts(1);
		particleVerts[0] = {0.0f, 0.0f, 0.0f};

		std::vector<glm::vec3> particleNorms(1);
		particleNorms[0] = { 0.0f, 0.0f, -1.0f };

		std::vector<glm::vec2> particlUVs(1);
		particlUVs[0] = { 0.0f, 0.0f };

		std::vector<float> vertBuffer((particleVerts.size() * 3) + (particleNorms.size() * 3) + (particlUVs.size() * 2));
		{
			vertBuffer[0] = particleVerts[0].x;
			vertBuffer[1] = particleVerts[0].y;
			vertBuffer[2] = particleVerts[0].z;

			vertBuffer[3] = particleNorms[0].x;
			vertBuffer[4] = particleNorms[0].y;
			vertBuffer[5] = particleNorms[0].z;

			vertBuffer[6] = particlUVs[0].x;
			vertBuffer[7] = particlUVs[0].y;
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

		//Sort the particles by active
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

		//Get camera position for distance sorting

		//Sort the particles
		glm::mat4 viewProjection = Camera::GetMain()->GetVPMatrix();
		qsort_s(
			(*particleSystemData).data(),
			enabledParticles.size(),
			sizeof(ParticleInstanceData),
			compareFunc_Depth_s,
			(void*)&viewProjection);

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

	static inline int compareFunc_Distance(void* cameraPosition, const void* left, const void* right)
	{
		double leftDistance = glm::length((*(ParticleInstanceData*)left).position - (*(glm::vec3*)cameraPosition));
		double rightDistance = glm::length((*(ParticleInstanceData*)right).position - (*(glm::vec3*)cameraPosition));

		return leftDistance <= rightDistance ? 1 : -1;
	}

	static inline int compareFunc_Depth_s(void* vpMatrix, const void* left, const void* right)
	{
		glm::mat4* Mat = ((glm::mat4*)vpMatrix);
		glm::vec4 L = glm::vec4((*(ParticleInstanceData*)left).position, 1.0);
		glm::vec4 R = glm::vec4((*(ParticleInstanceData*)right).position, 1.0);

		double leftDistance = (*(Mat))[2].x * L.x + (*(Mat))[2].y * L.y + (*(Mat))[2].z * L.z + (*(Mat))[2].w;
		double rightDistance = (*(Mat))[2].x * R.x + (*(Mat))[2].y * R.y + (*(Mat))[2].z * R.z + (*(Mat))[2].w;

		return leftDistance <= rightDistance ? 1 : -1;
	}
};