#include "GameObject.h"


GameObject::GameObject(Geometry *mesh, Material *mat)
{
	geometry = mesh;
	material = mat;

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	// Components //
	transform = new Transform();
	transform->SetOwner(this);

}


GameObject::~GameObject()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	const VkDescriptorPool descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();

	delete transform;

	for (int i = 0, count = components.size(); i < count; i++)
	{
		delete components[i];
	}
}

void GameObject::Draw(CommandBuffer &commandBuffer)
{
	const PipelineData pipelineData = material->GetPipelineData();
	const VkCommandBuffer vkCmdBuffer = commandBuffer.GetVKCommandBuffer();

	vkCmdBindPipeline(vkCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipeline);

	geometry->Draw(vkCmdBuffer);
}

void GameObject::Update()
{
	for (int i = 0, count = components.size(); i < count; i++)
	{
		components[i]->Update();
	}

	transform->Update(); //Transform must be the last component to update.
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);

	component->SetOwner(this);
	component->Start();
}
