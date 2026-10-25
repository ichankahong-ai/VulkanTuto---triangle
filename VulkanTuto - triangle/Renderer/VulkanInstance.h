#pragma once

#include "Constants.h"

class VulkanInstance
{
public:
	void createInstance();
	void setupDebugCallback();
	void createSurface(GLFWwindow* window);

	vk::raii::Instance& getInstance() { return instance; }
	vk::raii::SurfaceKHR& getSurface() { return surface; }

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;

	std::vector<const char*> GetRequiredExtension();
	std::vector<const char*> GetRequiredLayers();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
};
