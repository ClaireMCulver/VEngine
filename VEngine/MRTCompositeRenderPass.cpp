#include "MRTCompositeRenderPass.h"



MRTCompositeRenderPass::MRTCompositeRenderPass(uint32_t renderAreaWidth, uint32_t renderAreaHeight) : RenderPass(renderAreaWidth, renderAreaHeight)
{
	AddNewSubPass();

	//Main output
	VkAttachmentReference outputOutput = AddColourAttachmentToCurrentSubpass(renderAreaWidth, renderAreaHeight, VkFormat::VK_FORMAT_R8G8B8A8_UNORM);

	//Finalize renderpass?
	CreateRenderPass();
}


MRTCompositeRenderPass::~MRTCompositeRenderPass()
{
}
