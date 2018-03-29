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
#include "TextureMergingRenderer.scr"

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
	
	Shader lineVertShader("../Assets/Shaders/DrawingShader.vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader lineFragShader("../Assets/Shaders/DrawingShader.frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	Texture boxTex("../Assets/Textures/Smoke.png", 512, 512);

	Material lineMaterial;
	lineMaterial.AddShader(lineVertShader);
	lineMaterial.AddShader(lineFragShader);
	lineMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass(), mainRenderPass.GetVKDescriptorSetLayout(), mainRenderPass.GetVKPipelineLayout(), VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST);

	GameObject mainCamera(&cubeMesh, &lineMaterial);
	mainCamera.AddComponent(new Camera());
	mainCamera.GetTransform()->Translate(glm::vec3(0.0f, 0, -40.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 0, 0 });
	objectManager.AddObject(&mainCamera);

	GameObject mouseDrawer(&cubeMesh, &lineMaterial);
	mouseDrawer.AddComponent(new MouseDrawing());
	mouseDrawer.AddComponent(new DrawingRenderer());
	mainRenderPass.RegisterObject(&mouseDrawer, 0);
	objectManager.AddObject(&mouseDrawer);

	// merge textures //

	// I really don't like having a second render pass, rather than having a sub pass in the first one, but adding functionality that I like for that would take longer than I have to add.
	// So I have an addition one for compositing and one for ui compositing.
	DeferredRenderPass mergePass;

	Shader textureMergeVertShader("../Assets/Shaders/MergeTextures.vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
	Shader textureMergeFragShader("../Assets/Shaders/MergeTextures.frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

	Material MergeTexturesMaterial;
	MergeTexturesMaterial.AddShader(textureMergeVertShader);
	MergeTexturesMaterial.AddShader(textureMergeFragShader);
	MergeTexturesMaterial.FinalizeMaterial(mergePass.GetVKRenderPass(), mergePass.GetVKDescriptorSetLayout(), mergePass.GetVKPipelineLayout(), VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

	GameObject textureMergeEffect(&cubeMesh, &MergeTexturesMaterial);
	textureMergeEffect.AddComponent(new TextureMergingRenderer({ WindowSize[0], WindowSize[1] }, { WindowSize[0], WindowSize[1] }));
	mergePass.RegisterObject(&textureMergeEffect, 0);
	textureMergeEffect.GetComponent<TextureMergingRenderer>()->SetSourceTexture(mainRenderPass.GetRenderedImage(), mergePass.GetRenderedImage());
	objectManager.AddObject(&textureMergeEffect); 


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


		//Composite render
		textureMergeEffect.GetComponent<TextureMergingRenderer>()->UpdateNewDrawTextre(); //Again, really hate this for this specific reason. Don't have time for better.
		mergePass.RecordBuffer();
		mergePass.SubmitBuffer();
		mergePass.ResetBuffer();
		textureMergeEffect.GetComponent<TextureMergingRenderer>()->UpdateMergedTexture(); 


		swapchain.BlitToSwapChain(mergePass.GetRenderedImage());
	}

	graphicsSystem.WaitForDeviceIdle();

	return;
}