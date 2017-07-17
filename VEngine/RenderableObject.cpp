#include "RenderableObject.h"


RenderableObject::RenderableObject(Geometry *mesh, Material *mat)
{
	geometry = mesh;
	material = mat;

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();

	std::vector<VkDescriptorSetLayout>* uniformLayout = &material->GetDescriptorLayout();
	
	uniforms.resize(uniformLayout->size());
	
	VkDescriptorSetAllocateInfo descriptorSetCI;
	descriptorSetCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetCI.pNext = NULL;
	descriptorSetCI.descriptorPool = GraphicsSystem::GetSingleton()->GetDescriptorPool()->GetVKDescriptorPool();
	descriptorSetCI.descriptorSetCount = uniformLayout->size();
	descriptorSetCI.pSetLayouts = uniformLayout->data();

	vkAllocateDescriptorSets(logicalDevice, &descriptorSetCI, uniforms.data());
}


RenderableObject::~RenderableObject()
{
	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice()->GetVKLogicalDevice();
	const VkDescriptorPool descriptorPool = GraphicsSystem::GetSingleton()->GetDescriptorPool()->GetVKDescriptorPool();
	
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