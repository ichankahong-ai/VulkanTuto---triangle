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
	rend->waitIdle();
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, "hello triangle", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, Renderer::framebufferResizeCallback);
}

void Application::InitVulkan()
{
	rend = new Renderer();
	rend->setWindow(window);
	rend->init();
}

void Application::DestroyWindow()
{
	glfwDestroyWindow(window);

	glfwTerminate();
}

void Application::DestroyVulkan()
{
}
