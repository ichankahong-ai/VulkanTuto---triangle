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
	}
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
}

void Application::DestroyWindow()
{
	glfwDestroyWindow(window);

	glfwTerminate();
}

void Application::DestroyVulkan()
{


}


