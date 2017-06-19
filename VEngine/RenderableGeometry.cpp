#include "RenderableGeometry.h"



RenderableGeometry::RenderableGeometry()
{
}

RenderableGeometry::~RenderableGeometry()
{
	//Vertex buffer
	vkDestroyBuffer(GraphicsSystem::GetSingleton()->GetLogicalDevice(), vertexBuffer.buffer, NULL); 
	vkFreeMemory(GraphicsSystem::GetSingleton()->GetLogicalDevice(), vertexBuffer.memory, NULL);
}

bool RenderableGeometry::LoadMeshFromFile(char* filePath)
{

	return true;
}

bool RenderableGeometry::LoadMesh(std::vector<Triangle> *geometry)
{	
	//Create buffer
	VkBufferCreateInfo bufferInfo;
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; //What the buffer actually is
	bufferInfo.size = sizeof(Triangle) * geometry->size(); //size of the buffer in bytes
	bufferInfo.queueFamilyIndexCount = 0;
	bufferInfo.pQueueFamilyIndices = NULL;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.flags = 0;

	const VkDevice logicalDevice = GraphicsSystem::GetSingleton()->GetLogicalDevice();

	VkResult res = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &vertexBuffer.buffer);
	assert(res == VK_SUCCESS);

	//Fetch the memory requirements
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, vertexBuffer.buffer, &memRequirements);

	VkPhysicalDeviceMemoryProperties memoryProperties = GraphicsSystem::GetSingleton()->GetPhysicalMemoryProperties();

	//Allocate the memory
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.memoryTypeIndex = 0;
	allocInfo.allocationSize = memRequirements.size;
	bool pass = FindMemoryType(memoryProperties, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex);
	assert(pass); //currently, I have 999 as an error code. I should really put that as an enum, or use something else;

	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &vertexBuffer.memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(logicalDevice, vertexBuffer.buffer, vertexBuffer.memory, 0);

	void* data;
	vkMapMemory(logicalDevice, vertexBuffer.memory, 0, bufferInfo.size, 0, &data);
	memcpy(data, geometry->data(), (size_t)bufferInfo.size);
	vkUnmapMemory(logicalDevice, vertexBuffer.memory);

	return true;
}

bool RenderableGeometry::Render(VkCommandBuffer commandBuffer)
{
	PipelineData objectData = objectMaterial.GetPipelineData();
	
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, objectData.pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, objectData.pipelineLayout, 0, objectData.descriptors.size(),
		objectData.descriptors.data(), 0, NULL);

	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);

	vkCmdDraw(commandBuffer, 12 * 3, 1, 0, 0);

	return true;
}

bool RenderableGeometry::InitializeCommandBuffer()
{
	VkResult result;

	VkCommandBufferAllocateInfo cmdBufferAllocateInfo;
	cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocateInfo.pNext = NULL;
	cmdBufferAllocateInfo.commandPool = GraphicsSystem::GetSingleton()->GetCommandPool();
	cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	cmdBufferAllocateInfo.commandBufferCount = 1;
	result = vkAllocateCommandBuffers(GraphicsSystem::GetLogicalDevice(), &cmdBufferAllocateInfo, &renderCmdBuffer);

	assert(result == VK_SUCCESS);

	return true;
}

void RenderableGeometry::RecordCommandBuffer(VkRenderPass rndrPass)
{
	VkCommandBufferInheritanceInfo cmdBufferIICI;
	cmdBufferIICI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	cmdBufferIICI.pNext = NULL;
	cmdBufferIICI.renderPass = rndrPass;
	cmdBufferIICI.subpass = 0;
	cmdBufferIICI.framebuffer = VK_NULL_HANDLE; //specifying the index MAY result in better performance.
	cmdBufferIICI.occlusionQueryEnable = VK_FALSE;
	cmdBufferIICI.queryFlags = 0;
	cmdBufferIICI.pipelineStatistics = VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT;

	VkCommandBufferBeginInfo cmdBufferCI;
	cmdBufferCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferCI.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
	cmdBufferCI.pNext = NULL;
	cmdBufferCI.pInheritanceInfo = &cmdBufferIICI;

	vkBeginCommandBuffer(renderCmdBuffer, &cmdBufferCI);

	const PipelineData pipelineData = objectMaterial.GetPipelineData();

	vkCmdBindPipeline(renderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipeline);
	vkCmdBindDescriptorSets(renderCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pipelineLayout, 0, NUM_DESCRIPTOR_SETS,
		pipelineData.descriptors.data(), 0, NULL);

	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(renderCmdBuffer, 0, 1, &vertexBuffer.buffer, offsets);

	vkCmdDraw(renderCmdBuffer, 12 * 3, 1, 0, 0);
	vkCmdEndRenderPass(renderCmdBuffer);
}

void RenderableGeometry::RenderObject(VkCommandBuffer &primaryBuffer)
{
	vkCmdExecuteCommands(primaryBuffer, 1, &renderCmdBuffer);
}

bool RenderableGeometry::FindMemoryType(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex)
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}