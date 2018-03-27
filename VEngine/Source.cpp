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

#include "Rotate.cpp"
#include "Boid.cpp"
#include "FirstPersonControls.cpp"
#include "ParticleRenderer.scr"
#include "ParticleSystem.cpp"
#include "MeshRenderer.scr"
#include "MouseDrawing.scr"
#include "DrawingRenderer.scr"

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
	
	Shader standardVertShader("../Assets/Shaders/DrawingShader.vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader standardFragShader("../Assets/Shaders/DrawingShader.frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	Texture boxTex("../Assets/Textures/Smoke.png", 512, 512);

	Material standardMaterial;
	standardMaterial.AddShader(standardVertShader);
	standardMaterial.AddShader(standardFragShader);
	standardMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass(), mainRenderPass.GetVKDescriptorSetLayout(), mainRenderPass.GetVKPipelineLayout());

	GameObject mainCamera(&cubeMesh, &standardMaterial);
	mainCamera.AddComponent(new Camera());
	mainCamera.GetTransform()->Translate(glm::vec3(0.0f, 0, -40.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 0, 0 });
	objectManager.AddObject(&mainCamera);

	GameObject mouseDrawer(&cubeMesh, &standardMaterial);
	mouseDrawer.AddComponent(new MouseDrawing(mainRenderPass.GetRenderedImage()));
	mouseDrawer.AddComponent(new DrawingRenderer());
	mainRenderPass.RegisterObject(&mouseDrawer);
	objectManager.AddObject(&mouseDrawer);

	// Main loop //
	while (!inputSystem.GetKeyboard()->IsKeyDown('q'))
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