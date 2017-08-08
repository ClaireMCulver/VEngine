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

#include "GraphicsDefs.h"
#include "Graphics.h"
#include "SwapChain.h"
#include "DeferredRenderPass.h"
#include "RenderableObject.h"
#include "Material.h"
#include "Texture.h"

#include "WinKeyboard.h"

void main()
{
	// Input //
	WinKeyboard keyboard;

	// Vulkan graphics //
	GraphicsSystem graphicsSystem;
	SwapChain swapchain = SwapChain(graphicsSystem, 800, 600, false);
	DescriptorPool descriptorPool;

	// Objects //
	DeferredRenderPass mainRenderPass;
	
	Geometry cubeMesh;
	cubeMesh.LoadMeshFromDae("../Assets/Models/box.dae");
	
	Shader standardVertShader("../Assets/Shaders/StandardVertShader.glsl", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader standardFragShader("../Assets/Shaders/StandardFragShader.glsl", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	Texture renderTex("../Assets/Textures/box.png", 512, 512);

	Material standardMaterial;
	standardMaterial.AddShader(standardVertShader);
	standardMaterial.AddShader(standardFragShader);
	standardMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass());
	
	RenderableObject cube(&cubeMesh, &standardMaterial);

	// Object variable setting //
	cube.SetTexture(renderTex, 1, 0);

	glm::mat4x4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	glm::mat4x4 View = glm::lookAt(glm::vec3(-3, 3, -8),  // Camera is at (-5,3,-10), in World Space
		glm::vec3(0, 0, 0),     // and looks at the origin
		glm::vec3(0, 1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4x4 Model = glm::mat4(1.0f);

	// Vulkan clip space has inverted Y and half Z.
	glm::mat4x4 Clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f);
	glm::mat4x4 MVP = Clip * Projection * View * Model;
	
	cube.SetUniform_Mat4x4(MVP, 0, 0);

	mainRenderPass.RegisterObject(cube);

	// Main loop //
	while (true)
	{
		 //Physical update
		keyboard.UpdateKeyboardState();

		Model = glm::rotate(Model, 0.0005f, glm::vec3(0, 1, 0));
		MVP = Clip * Projection * View * Model;

		cube.SetUniform_Mat4x4(MVP, 0, 0);


		//Render update
		mainRenderPass.RecordBuffer();
		mainRenderPass.SubmitBuffer();

		mainRenderPass.ResetBuffer();

		swapchain.BlitToSwapChain(mainRenderPass.GetRenderedImage());
	}

	graphicsSystem.WaitForDeviceIdle();

	return;
}