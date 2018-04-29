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
#include "FirstPersonControls.cpp"
#include "ParticleRenderer.scr"
#include "ParticleSystem.cpp"
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
	
	Shader particleVertShader("../Assets/Shaders/ParticleShader.vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader particleGeomShader("../Assets/Shaders/ParticleShader.geom", VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT);
	Shader particleFragShader("../Assets/Shaders/ParticleShader.frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	Texture boxTex("../Assets/Textures/Smoke.png", 512, 512);

	Material lineMaterial;
	lineMaterial.AddShader(particleVertShader);
	lineMaterial.AddShader(particleGeomShader);
	lineMaterial.AddShader(particleFragShader);
	lineMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass(), mainRenderPass.GetVKDescriptorSetLayout(), mainRenderPass.GetVKPipelineLayout(), VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST);

	GameObject mainCamera(&cubeMesh, &lineMaterial);
	mainCamera.AddComponent(new Camera());
	FirstPersonControls* aFirstPersonThing = new FirstPersonControls();
	mainCamera.AddComponent(aFirstPersonThing);
	mainCamera.GetTransform()->Translate(glm::vec3(0.0f, 0, -40.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 0, 0 });
	objectManager.AddObject(&mainCamera);

	GameObject particleEmitter(&cubeMesh, &lineMaterial);
	particleEmitter.SetTexture(boxTex, 0);
	particleEmitter.AddComponent(new ParticleSystem(1000));
	particleEmitter.AddComponent(new ParticleRenderer());
	objectManager.AddObject(&particleEmitter);
	mainRenderPass.RegisterObject(&particleEmitter, 0);


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