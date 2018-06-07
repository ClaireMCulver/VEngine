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

#include "fxaaEffect.cpp"
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
	mainCamera.AddComponent(new Camera());
	mainCamera.AddComponent(new FirstPersonControls());
	mainCamera.GetTransform()->Translate(glm::vec3(0.0f, 0, -40.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 0, 0 });
	objectManager.AddObject(&mainCamera);

	GameObject clayObject(&cubeMesh, &bitangentLightingMaterial);
	clayObject.GetTransform()->Translate({ 0, 0, 0 });
	clayObject.SetTexture(boxTex, 0);
	clayObject.SetTexture(normTex, 1);
	clayObject.SetTexture(normTex, 2); //And this is why I need defaulting setups everywhere.
	clayObject.SetTexture(normTex, 3);
	clayObject.SetTexture(normTex, 4);
	clayObject.SetTexture(normTex, 5);
	clayObject.AddComponent(new RotateScript());
	clayObject.AddComponent(new MeshRenderer());
	objectManager.AddObject(&clayObject);
	mainRenderPass.RegisterObject(&clayObject, 0);

	//fxaa effect
	DeferredRenderPass fxaaPass(WindowSize[0], WindowSize[1]);

	Shader textureMergeVertShader("../Assets/Shaders/ScreenEffectPassthrough.vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader textureMergeFragShader("../Assets/Shaders/fxaa.frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	
	Material fxaaMaterial;
	fxaaMaterial.AddShader(textureMergeVertShader);
	fxaaMaterial.AddShader(textureMergeFragShader);
	fxaaMaterial.FinalizeMaterial(fxaaPass.GetVKRenderPass(), fxaaPass.GetVKDescriptorSetLayout(), fxaaPass.GetVKPipelineLayout(), VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

	GameObject textureMergeEffect(&cubeMesh, &fxaaMaterial);
	textureMergeEffect.AddComponent(new FXAARenderer({ WindowSize[0], WindowSize[1] }, { WindowSize[0], WindowSize[1] }));
	textureMergeEffect.GetComponent<FXAARenderer>()->SetSourceTexture(mainRenderPass.GetImage(0));
	objectManager.AddObject(&textureMergeEffect);
	fxaaPass.RegisterObject(&textureMergeEffect, 0);
	
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

		fxaaPass.RecordBuffer();
		fxaaPass.SubmitBuffer();
		fxaaPass.ResetBuffer();

		swapchain.BlitToSwapChain(fxaaPass.GetImage(0));
	}

	graphicsSystem.WaitForDeviceIdle();

	return;
}