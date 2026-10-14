#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "vulkan/vulkan_raii.hpp"

class Renderer
{
public:

	void createInstance();
	void pickPhysicalDevice();
	void createLogicalDevice();

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;

	std::vector<const char*> GetRequiredExtension();
	std::vector<const char*> GetRequiredLayers();
	std::vector<const char*> GetRequiredSwapchain();

	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);

	void SetupDebugCallback();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
};

