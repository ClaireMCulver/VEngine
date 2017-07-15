#include "DeferredRenderPass.h"



DeferredRenderPass::DeferredRenderPass()
{
	AddNewSubPass();

	//Main output
	AddColourAttachementToCurrentSubpass(WindowSize[0], WindowSize[1], VkFormat::VK_FORMAT_R8G8B8A8_UNORM, true);

	//Finalize renderpass?
	CreateRenderPass();
}


DeferredRenderPass::~DeferredRenderPass()
{
}
