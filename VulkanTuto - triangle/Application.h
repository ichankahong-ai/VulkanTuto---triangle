#pragma once
#include "includevk.h"
#include "Renderer.h"

class Application
{
public:
	int width = 800;
	int height = 600;

	GLFWwindow* window = nullptr;

	Renderer *rend = nullptr;

	void Init();
	void Run();
	void Destroy();

private:
	void InitWindow();
	void InitVulkan();

	void recreateSwapChain();
	void cleanupSwapChain();

	void DestroyWindow();
	void DestroyVulkan();

};

