#include "GameObject.h"


GameObject::GameObject(Geometry *mesh, Material *mat)
{
	geometry = mesh;
	material = mat;

	// Components //
	transform = new Transform();
	transform->SetOwner(this);

	material->SetOwner(this);
}


GameObject::~GameObject()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	const VkDescriptorPool descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();

	delete transform;

	//Iterate through the components, delete each component, which is the second stored value in the map
	for (std::pair<const std::type_info*, Component*> element : components)
	{
		element.second->~Component();
	}
}

void GameObject::SetRenderer(Renderer * givenRenderer)
{
	assert(renderer == nullptr);

	renderer = givenRenderer;

	ObjectManager::GetManager()->AddRenderObject(this);
}

void GameObject::Update()
{
	//Iterate through the components, update each component, which is the second stored value in the map
	for (std::pair<const std::type_info*, Component*> element : components)
	{
		element.second->Update();
	}

	transform->Update(); //Transform must be the last component to update.
}

void GameObject::AddComponent(Component* component)
{
	components.insert(std::make_pair(&typeid(*component), component));

	component->SetOwner(this);
	component->Start();
}
