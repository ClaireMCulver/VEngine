#include "GameObject.h"


GameObject::GameObject(Geometry *mesh, Material *mat)
{
	geometry = mesh;
	material = mat;

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	// Components //
	transform = new Transform();
	transform->SetOwner(this);

	// Uniforms 
	uniformBuffer = new UniformBuffer();

	//Per-Draw Data
	descriptorBindings.push_back
	(
	{	//VkDescriptorSetLayoutBinding
		0,											//binding;				
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			//descriptorType;		
		1,											//descriptorCount;		
		VkShaderStageFlagBits::VK_SHADER_STAGE_ALL,	//stageFlags;			
		NULL										//pImmutableSamplers;
	}
	);

	//Passed Textures
	descriptorBindings.push_back
	(
	{	//VkDescriptorSetLayoutBinding
		1,											//binding;				
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	//descriptorType;		
		1,											//descriptorCount;		
		VkShaderStageFlagBits::VK_SHADER_STAGE_ALL,	//stageFlags;			
		NULL										//pImmutableSamplers;
	}
	);

	// Descriptor Set Layout //
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI;
	descriptorSetLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCI.pNext = NULL;
	descriptorSetLayoutCI.flags = 0;
	descriptorSetLayoutCI.bindingCount = descriptorBindings.size();
	descriptorSetLayoutCI.pBindings = descriptorBindings.data();

	vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCI, NULL, &descriptorSetLayout);

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = NULL;
	descriptorSetAllocateInfo.descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &uniformDescriptorSet);

	//Textures
	textures.reserve(5);

	//Instance buffer.
	//This solution is a temporary one. I'd like to organize this better.
	instanceBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(glm::vec3) + sizeof(glm::quat));

}


GameObject::~GameObject()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	const VkDescriptorPool descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();

	delete transform;

	delete uniformBuffer;

	//Iterate through the components, delete each component, which is the second stored value in the map
	for (std::pair<const std::type_info*, Component*> element : components)
	{
		delete element.second;
	}

	delete instanceBuffer;

	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, NULL);
}

void GameObject::Draw(CommandBuffer &commandBuffer, VkPipelineLayout pipelineLayout)
{
	const PipelineData pipelineData = material->GetPipelineData();
	const VkCommandBuffer vkCmdBuffer = commandBuffer.GetVKCommandBuffer();

	vkCmdBindPipeline(vkCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipeline);
	BindPerDrawUniforms(vkCmdBuffer, pipelineLayout);

	geometry->Draw(vkCmdBuffer);
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

//Should be a pointer to a heap component. Will be deleted by the GameObject.
void GameObject::AddComponent(Component* component)
{
	components.insert(std::make_pair(&typeid(*component), component));

	component->SetOwner(this);
	component->Start();
}

void GameObject::UpdateDescriptorSet()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = uniformBuffer->GetVKBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet descriptorWrite;
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = NULL;
	descriptorWrite.dstSet = uniformDescriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.pImageInfo = NULL;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(logicalDevice, 1, &descriptorWrite, 0, NULL);

	VkDescriptorImageInfo imageInfo;
	for (size_t i = 0, count = textures.size(); i < count; i++)
	{
		//Image infor the describes the image to 
		imageInfo.sampler = textures[i]->GetSampler();
		imageInfo.imageView = textures[i]->GetImageView();
		imageInfo.imageLayout = textures[i]->GetImageLayout();

		//Descriptor write that tells Vulkan what we're updating and what we're updating it with
		uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		uniformWrite.pNext = NULL;
		uniformWrite.dstSet = uniformDescriptorSet;
		uniformWrite.dstBinding = PerDrawUniformTextureBinding;
		uniformWrite.dstArrayElement = i; //TODO: Figure out what the fuck.
		uniformWrite.descriptorCount = 1;
		uniformWrite.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		uniformWrite.pImageInfo = &imageInfo;
		uniformWrite.pBufferInfo = NULL;
		uniformWrite.pTexelBufferView = NULL;

		vkUpdateDescriptorSets(logicalDevice, 1, &uniformWrite, 0, NULL);
	}
}

void GameObject::BindPerDrawUniforms(VkCommandBuffer vkRenderBuffer, VkPipelineLayout pipelineLayout)
{
	vkCmdBindDescriptorSets(vkRenderBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &uniformDescriptorSet, 0, NULL);
}

void GameObject::SetDrawMatrices(glm::mat4 &modelMat, glm::mat4 &viewMat, glm::mat4 &viewProjoectionMat)
{
	size_t mat4Size = sizeof(glm::mat4);

	//Model Matrix
	SetUniform_Mat4x4(modelMat, 0);

	//Model View Matrix
	SetUniform_Mat4x4(viewMat * modelMat, mat4Size);

	//Model View Projection Matrix
	SetUniform_Mat4x4(viewProjoectionMat * modelMat, mat4Size + mat4Size);

	instanceData.position = transform->GetPosition();
	instanceData.rotation = glm::toQuat(modelMat);
	instanceBuffer->CopyMemoryIntoBuffer(&instanceData, sizeof(InstanceData));
}

void GameObject::SetUniform_Mat4x4(glm::mat4x4 &data, int offset)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//Place the data into the uniform buffer
	uniformBuffer->SetBufferData(&data, sizeof(data), offset);
}

void GameObject::SetUniform_Int32(int &data, int offset)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//Place the data into the uniform buffer
	uniformBuffer->SetBufferData(&data, sizeof(data), offset);
}

void GameObject::SetUniform_Float32(float &data, int offset)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	//Place the data into the uniform buffer
	uniformBuffer->SetBufferData(&data, sizeof(data), offset);
}

void GameObject::SetTexture(Texture& texture, int offset)
{
	textures.push_back(&texture);
}

