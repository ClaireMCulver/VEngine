#pragma once

//cpp core
#include <vector>

//vulkan definitions
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include "GraphicsDefs.h"

//Engine
#include "RenderPass.h"

#include "GameDefs.h"

class DeferredRenderPass : public RenderPass
{
public:
	DeferredRenderPass();
	~DeferredRenderPass();
};

