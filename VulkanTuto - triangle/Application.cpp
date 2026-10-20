#include "Application.h"

void Application::Init()
{
	InitWindow();
	InitVulkan();
}

void Application::Run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		rend->drawFrame();
	}
	rend->device.waitIdle();
}

void Application::Destroy()
{
	DestroyVulkan();
	DestroyWindow();
}

void Application::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, "hello triangle", nullptr, nullptr);
}

void Application::InitVulkan()
{
	rend = new Renderer();
	rend->setWindow(window);

	rend->createInstance();
	rend->SetupDebugCallback();
	rend->createSurface();
	rend->pickPhysicalDevice();
	rend->createLogicalDevice();
	rend->createSwapChain();
	rend->createImageViews();
	rend->createGraphicsPipeline();
	rend->createCommandPool();
	rend->createCommandBuffers();
	rend->createSyncObjects();
}

void Application::recreateSwapChain()
{
	rend->device.waitIdle();

	cleanupSwapChain();
	rend->createSwapChain();
	rend->createImageViews();
}

void Application::cleanupSwapChain()
{
	rend->swapChainImageViews.clear();
	rend->swapChain = nullptr;
}

void Application::DestroyWindow()
{
	cleanupSwapChain();

	glfwDestroyWindow(window);

	glfwTerminate();
}

void Application::DestroyVulkan()
{


}


