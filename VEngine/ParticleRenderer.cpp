#pragma

#include "Renderer.cpp"
#include "ParticleEmitter.cpp"

class ParticleRenderer : public Renderer
{
public:

	ParticleRenderer()
	{
	}

	~ParticleRenderer()
	{

	}

	void Start()
	{
		particleEmitter = owner->GetComponent<ParticleEmitter>();
		owner->SetRenderer(this);
	}

	void Update()
	{

	}

	void Draw(VkCommandBuffer commandBuffer)
	{
		if (particleEmitter->NumActiveParticles() > 0)
		{
			RenderMesh(commandBuffer, 6, particleEmitter->vertexBuffer->GetVKBuffer(), particleEmitter->instanceBuffer->GetVKBuffer(), particleEmitter->NumActiveParticles());
		}
	}

private:
	ParticleEmitter* particleEmitter;
};
