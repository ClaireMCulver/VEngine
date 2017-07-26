#include "RenderableObject.h"


RenderableObject::RenderableObject(Geometry *mesh, Material *mat)
{
	geometry = mesh;
	material = mat;

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	std::vector<VkDescriptorSetLayout> uniformLayout = material->GetDescriptorLayout();
	
	// Uniform allocations //
	uniforms.resize(uniformLayout.size());
	
	VkDescriptorSetAllocateInfo descriptorSetCI;
	descriptorSetCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetCI.pNext = NULL;
	descriptorSetCI.descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();
	descriptorSetCI.descriptorSetCount = uniformLayout.size();
	descriptorSetCI.pSetLayouts = uniformLayout.data();

	vkAllocateDescriptorSets(logicalDevice, &descriptorSetCI, uniforms.data());

	// Uniform Buffer //
	uniformBuffer = new GPUBuffer(VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(glm::mat4x4));
}


RenderableObject::~RenderableObject()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	const VkDescriptorPool descriptorPool = DescriptorPool::GetSingleton()->GetVKDescriptorPool();

	delete uniformBuffer;
	
	vkFreeDescriptorSets(logicalDevice, descriptorPool, uniforms.size(), uniforms.data());
}

void RenderableObject::Draw(CommandBuffer &commandBuffer)
{
	const PipelineData pipelineData = material->GetPipelineData();
	const VkCommandBuffer vkCmdBuffer = commandBuffer.GetVKCommandBuffer();

	vkCmdBindPipeline(vkCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipeline);

	vkCmdBindDescriptorSets(vkCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipelineLayout, 0, uniforms.size(), uniforms.data(), 0, NULL);

	geometry->Draw(vkCmdBuffer);
}

void RenderableObject::SetUniform_Mat4x4(glm::mat4x4 &data, int uniformSet, int binding)
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	uint64_t bufferSize = sizeof(data);

	//Place the data into the uniform buffer
	uniformBuffer->CopyMemoryIntoBuffer(&data, bufferSize);

	//Buffer info that describes how to use the buffer to update the uniform
	VkDescriptorBufferInfo uniformBufferInfo;
	uniformBufferInfo.buffer = uniformBuffer->GetVKBuffer();
	uniformBufferInfo.offset = 0;
	uniformBufferInfo.range = bufferSize;
	
	//Descriptor write that tells Vulkan what we're updating and what we're updating it with
	uniformWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	uniformWrite.pNext = NULL;
	uniformWrite.dstSet = uniforms[uniformSet];
	uniformWrite.dstBinding = binding;
	uniformWrite.dstArrayElement = 0;
	uniformWrite.descriptorCount = 1;
	uniformWrite.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformWrite.pImageInfo = NULL;
	uniformWrite.pBufferInfo = &uniformBufferInfo;
	uniformWrite.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(logicalDevice, 1, &uniformWrite, 0, NULL);
}