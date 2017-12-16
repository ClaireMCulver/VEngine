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
#include "ParticleRenderer.scr"
#include "ParticleEmitter.scr"
#include "MeshRenderer.scr"

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
	
	Shader standardVertShader("../Assets/Shaders/StandardParticleVertShader.glsl", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader standardFragShader("../Assets/Shaders/StandardParticleFragShader.glsl", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	Texture boxTex("../Assets/Textures/box.png", 512, 512);

	Material standardMaterial;
	standardMaterial.AddShader(standardVertShader);
	standardMaterial.AddShader(standardFragShader);
	standardMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass(), mainRenderPass.GetVKDescriptorSetLayout(), mainRenderPass.GetVKPipelineLayout());

	GameObject mainCamera(&cubeMesh, &standardMaterial);
	mainCamera.AddComponent(new Camera());
	mainCamera.GetTransform()->Translate(glm::vec3(0.0f, 15, -40.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 15, 0 });
	mainCamera.AddComponent(new FirstPersonControls());
	objectManager.AddObject(&mainCamera);

	GameObject particleSystem(&cubeMesh, &standardMaterial);
	particleSystem.SetTexture(boxTex, 0);
	particleSystem.AddComponent(new ParticleEmitter(1000));
	particleSystem.AddComponent(new ParticleRenderer());
	mainRenderPass.RegisterObject(&particleSystem);
	objectManager.AddObject(&particleSystem);

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