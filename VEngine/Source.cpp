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
#include "GameObject.h"
#include "Material.h"
#include "Texture.h"
#include "ObjectManager.h"

#include "Input.h"
#include "Clock.h"
#include "Camera.h"

#include "Rotate.scr"
#include "Boid.scr"
#include "FirstPersonControls.scr"

void main()
{
	// Clock //
	Clock clock;

	// Input //
	Input inputSystem;

	// Vulkan graphics //
	GraphicsSystem graphicsSystem;
	SwapChain swapchain = SwapChain(graphicsSystem, 800, 600, false);
	DescriptorPool descriptorPool;

	// Objects //
	ObjectManager objectManager;

	DeferredRenderPass mainRenderPass;
	
	Geometry cubeMesh;
	cubeMesh.LoadMeshFromDae("../Assets/Models/monkey.dae");
	
	Shader standardVertShader("../Assets/Shaders/StandardVertShader.glsl", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader standardFragShader("../Assets/Shaders/StandardFragShader.glsl", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	Texture boxTex("../Assets/Textures/box.png", 512, 512);

	Material standardMaterial;
	standardMaterial.AddShader(standardVertShader);
	standardMaterial.AddShader(standardFragShader);
	standardMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass(), mainRenderPass.GetVKDescriptorSetLayout(), mainRenderPass.GetVKPipelineLayout());

	GameObject mainCamera(&cubeMesh, &standardMaterial);
	mainCamera.AddComponent(new Camera());
	mainCamera.GetTransform()->Translate(glm::vec3(0.0f, 1, -10.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 2, 0 });
	mainCamera.AddComponent(new FirstPersonControls());
	objectManager.AddObject(&mainCamera);

	GameObject monkey(&cubeMesh, &standardMaterial);
	monkey.SetTexture(boxTex, 0);
	mainRenderPass.RegisterObject(&monkey);
	objectManager.AddObject(&monkey);

	// Main loop //
	while (!inputSystem.keyboard.IsKeyDown('q'))
	{
		//Clock update
		clock.Tick();

		//Window update
		inputSystem.UpdateInput();

		 //Physical update
		objectManager.UpdateObjects();

		//Render update
		mainRenderPass.RecordBuffer();
		mainRenderPass.SubmitBuffer();

		mainRenderPass.ResetBuffer();

		swapchain.BlitToSwapChain(mainRenderPass.GetRenderedImage());
	}

	graphicsSystem.WaitForDeviceIdle();

	return;
}