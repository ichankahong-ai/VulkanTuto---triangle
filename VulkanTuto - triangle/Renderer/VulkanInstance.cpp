#include "VulkanInstance.h"
#include "Debug.h"
#include <print>
#include <cstring>
#include <stdexcept>

void VulkanInstance::createInstance()
{
	constexpr vk::ApplicationInfo application_info
	{
		.pApplicationName = "YAY SPIN",
		.applicationVersion = VK_MAKE_VERSION(1,0,0),
		.pEngineName = "Gaming campus vulkan",
		.engineVersion = VK_MAKE_VERSION(1,0,0),
		.apiVersion = vk::ApiVersion14
	};

	std::vector requiredExtensions = GetRequiredExtension();

	auto extensionProperties = context.enumerateInstanceExtensionProperties();

	for (uint32_t i = 0; i < requiredExtensions.size(); i++)
	{
		auto compareExtensions = [glfwExtension = requiredExtensions[i]](auto const& extensionProperty)

			{
				return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
			};

		if (std::ranges::none_of(extensionProperties, compareExtensions))
		{
			throw std::runtime_error("required GLFW extension not supported: " + std::string(requiredExtensions[i]));

		}
	}

	std::vector<char const*> requiredLayers = GetRequiredLayers();

	auto layerProperties = context.enumerateInstanceLayerProperties();

	auto unsupportedLayerIt = std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer)
		{
			return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
				{
					return strcmp(layerProperty.layerName, requiredLayer) == 0;
				});
		});

	if (unsupportedLayerIt != requiredLayers.end())
	{
		throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
	}

	vk::InstanceCreateInfo create_info
	{
		.pApplicationInfo = &application_info,
		.enabledLayerCount = static_cast<uint32_t> (requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t> (requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()

	};

	instance = vk::raii::Instance(context, create_info);
}

void VulkanInstance::setupDebugCallback()
{
	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	vk::DebugUtilsMessengerCreateInfoEXT debugInfo{ .messageSeverity = severityFlags, .messageType = messageTypeFlags, .pfnUserCallback = &DebugCallBack };

	debugMessenger = instance.createDebugUtilsMessengerEXT(debugInfo);
}

vk::Bool32 VulkanInstance::DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cout << "Validation layer: Type [" << vk::to_string(type) << "] - Message : " << pCallbackData->pMessage << std::endl;

	std::println(std::cout, "Validation layer: Type [{}]", to_string(type));
	std::println(std::cout, "Message : {}", pCallbackData->pMessage);

	return vk::False;
}

void VulkanInstance::createSurface(GLFWwindow* window)
{
	VkSurfaceKHR	_surface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
		throw std::runtime_error("failed to create window surface!");
	}
	surface = vk::raii::SurfaceKHR(instance, _surface);
}

std::vector<const char*> VulkanInstance::GetRequiredExtension()
{
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<char const*> requiredExtensions = { vk::EXTDebugUtilsExtensionName };

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	extensions.append_range(requiredExtensions);

	return extensions;
}

std::vector<const char*> VulkanInstance::GetRequiredLayers()
{
	const std::vector ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	std::vector<char const*> requiredLayers = {};

	if (Debug::enableDebug)
		requiredLayers.append_range(ValidationLayers);

	return requiredLayers;
}
