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
#include "FXAAEffect.scr"

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

	DeferredRenderPass mainRenderPass(WindowSize[0], WindowSize[1]);
	
	Geometry cubeMesh;
	cubeMesh.LoadMeshFromDae("../Assets/Models/box.dae");
	
	Shader standardVertShader("../Assets/Shaders/StandardShader.vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader clayFragShader("../Assets/Shaders/BitangentLighting.frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

	Texture boxTex("../Assets/Textures/box.png", 512, 512);
	Texture normTex("../Assets/Textures/NormalsBox.png", 1024, 1024);

	Material bitangentLightingMaterial;
	bitangentLightingMaterial.AddShader(standardVertShader);
	bitangentLightingMaterial.AddShader(clayFragShader);
	bitangentLightingMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass(), mainRenderPass.GetVKDescriptorSetLayout(), mainRenderPass.GetVKPipelineLayout(), VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	GameObject mainCamera(&cubeMesh, &bitangentLightingMaterial);
	mainCamera.AddComponent(new Camera(&mainRenderPass));
	mainCamera.AddComponent(new FirstPersonControls());
	mainCamera.GetTransform()->Translate(glm::vec3(0.0f, 0, -10.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 0, 0 });
	objectManager.AddObject(&mainCamera);

	GameObject clayObject(&cubeMesh, &bitangentLightingMaterial);
	clayObject.GetTransform()->Translate({ 0, 0, 0 });
	bitangentLightingMaterial.SetTexture(boxTex, 0);
	bitangentLightingMaterial.SetTexture(normTex, 1);
	bitangentLightingMaterial.SetTexture(normTex, 2); //And this is why I need defaulting setups everywhere.
	bitangentLightingMaterial.SetTexture(normTex, 3);
	bitangentLightingMaterial.SetTexture(normTex, 4);
	bitangentLightingMaterial.SetTexture(normTex, 5);
	clayObject.AddComponent(new RotateScript());
	clayObject.AddComponent(new MeshRenderer());
	objectManager.AddObject(&clayObject);

	//fxaa effect
	mainCamera.AddComponent(new FXAAEffect());
	
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
		mainCamera.GetComponent<Camera>()->Render();

		swapchain.BlitToSwapChain(Camera::GetMain()->GetFinalOutput());
	}

	graphicsSystem.WaitForDeviceIdle();

	return;
}