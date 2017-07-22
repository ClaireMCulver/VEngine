#include <iostream>
#include <cstdlib>
#include <assert.h>

#include <vector>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

//vulkan definitions
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan\vulkan.h"

#include "Graphics.h"
#include "SwapChain.h"
#include "DeferredRenderPass.h"
#include "RenderableObject.h"
#include "Material.h"

#include "GraphicsDefs.h"

void main()
{
	GraphicsSystem graphicsSystem;
	SwapChain swapchain = SwapChain(graphicsSystem, 800, 600, false);
	DescriptorPool descriptorPool;

	DeferredRenderPass mainRenderPass;
	
	Geometry cubeMesh;
	cubeMesh.LoadMesh(g_vb_texture_Data);
	
	Shader standardVertShader(vertShaderText.data(), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader standardFragShader(fragShaderText.data(), VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	
	//Reinclude material.h in the project for compilation.
	//Material standardMaterial;
	//standardMaterial.AddShader(standardVertShader);
	//standardMaterial.AddShader(standardFragShader);
	//standardMaterial.FinalizeMaterial(mainRenderPass);
	
	
	//
	//RenderableObject cube(&cubeMesh, &standardMaterial);


	return;
}