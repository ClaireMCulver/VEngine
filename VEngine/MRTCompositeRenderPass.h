#pragma once

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

class MRTCompositeRenderPass : public RenderPass
{
public:
	MRTCompositeRenderPass(uint32_t renderAreaWidth, uint32_t renderAreaHeight);
	~MRTCompositeRenderPass();
};