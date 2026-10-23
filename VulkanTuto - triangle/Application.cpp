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
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, rend->framebufferResizeCallback);
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
	rend->createDescriptorSetLayout();
	rend->createGraphicsPipeline();
	rend->createCommandPool();
	rend->createTextureImage();
	rend->createTextureImageView();
	rend->createTextureSampler();
	rend->createVertexBuffer();
	rend->createIndexBuffer();
	rend->createUniformBuffers();
	rend->createDescriptorPool();
	rend->createDescriptorSets();
	rend->createCommandBuffers();
	rend->createSyncObjects();
}



void Application::DestroyWindow()
{
	rend->cleanupSwapChain();

	glfwDestroyWindow(window);

	glfwTerminate();
}

void Application::DestroyVulkan()
{


}


