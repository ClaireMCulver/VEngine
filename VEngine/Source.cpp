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
	Texture blueTribeTex("../Assets/Textures/box.png", 512, 512);
	Texture redTribeTex("../Assets/Textures/CyclesRender.png", 1920, 1080);

	Material standardMaterial;
	standardMaterial.AddShader(standardVertShader);
	standardMaterial.AddShader(standardFragShader);
	standardMaterial.FinalizeMaterial(mainRenderPass.GetVKRenderPass(), mainRenderPass.GetVKDescriptorSetLayout(), mainRenderPass.GetVKPipelineLayout());

	//Obstacles
	GameObject* Obstacles[5];
	Geometry ObstacleMesh;
	ObstacleMesh.LoadMeshFromDae("../Assets/Models/tree.dae");
	for (int i = 0; i < 5; i++)
	{
		Obstacles[i] = new GameObject(&ObstacleMesh, &standardMaterial);
		Obstacles[i]->SetTexture(blueTribeTex, 0);
		Obstacles[i]->GetTransform()->SetPosition({ 10 * i - 20, 0, 10 * i - 20 });

		mainRenderPass.RegisterObject(Obstacles[i]);
		objectManager.AddObject(Obstacles[i]);
	}

	//BlueTribe
	GameObject* blueTribe[5];
	for (int i = 0; i < 5; i++)
	{
		blueTribe[i] = new GameObject(&cubeMesh, &standardMaterial);
		blueTribe[i]->SetTexture(blueTribeTex, 0);
	
		blueTribe[i]->GetTransform()->SetPosition({ (10 * i) - 25, 0, -25 });
	
		blueTribe[i]->AddComponent(new Boid());
		blueTribe[i]->GetComponent<Boid>()->SetGoal({ 0, 0, 25 });

		mainRenderPass.RegisterObject(blueTribe[i]);
		objectManager.AddObject(blueTribe[i]);
	}


	//RedTribe
	GameObject* redTribe[5];
	for (int i = 0; i < 5; i++)
	{
		redTribe[i] = new GameObject(&cubeMesh, &standardMaterial);
		redTribe[i]->SetTexture(redTribeTex, 0);

		redTribe[i]->GetTransform()->SetPosition({ (12.7 * (float)i) - 25, 0, 25 });

		redTribe[i]->AddComponent(new Boid());
		redTribe[i]->GetComponent<Boid>()->SetGoal({ 0, 0, -25 });

		mainRenderPass.RegisterObject(redTribe[i]);
		objectManager.AddObject(redTribe[i]);
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			blueTribe[i]->GetComponent<Boid>()->AddObstacle(Obstacles[j]->GetTransform());
			blueTribe[i]->GetComponent<Boid>()->AddOtherTribeMember(redTribe[j]->GetComponent<Boid>());
			if (blueTribe[i] == blueTribe[j])
			{
				continue;
			}
			blueTribe[i]->GetComponent<Boid>()->AddFellowTribeMember(blueTribe[j]->GetComponent<Boid>());
		}
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			redTribe[i]->GetComponent<Boid>()->AddObstacle(Obstacles[j]->GetTransform());
			redTribe[i]->GetComponent<Boid>()->AddOtherTribeMember(blueTribe[j]->GetComponent<Boid>());
			if (redTribe[i] == redTribe[j])
			{
				continue;
			}
			redTribe[i]->GetComponent<Boid>()->AddFellowTribeMember(redTribe[j]->GetComponent<Boid>());
		}
	}

	GameObject mainCamera(&cubeMesh, &standardMaterial);
	mainCamera.AddComponent(new Camera());
	mainCamera.GetTransform()->Translate(glm::vec3(-10.0f, 80, 0.0f));
	mainCamera.GetComponent<Camera>()->SetLookPoint({ 0, 0, 0 });

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

	for (int i = 0; i < 5; i++)
	{
		delete blueTribe[i];
		delete redTribe[i];
		delete Obstacles[i];
	}

	graphicsSystem.WaitForDeviceIdle();

	return;
}