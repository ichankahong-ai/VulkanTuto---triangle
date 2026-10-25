#pragma once

#include "Constants.h"
#include <utility>

class VulkanDevice
{
public:
	void pickPhysicalDevice(vk::raii::Instance& instance);
	void createLogicalDevice(vk::raii::SurfaceKHR& surface);

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	vk::Format findDepthFormat();

	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
	vk::raii::ImageView createImageView(vk::Image const& image, vk::Format format, vk::ImageAspectFlags aspectFlags);

	vk::raii::PhysicalDevice& getPhysicalDevice() { return physicalDevice; }
	vk::raii::Device& getDevice() { return device; }
	vk::raii::Queue& getQueue() { return queue; }
	uint32_t getQueueIndex() const { return queueIndex; }

private:
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	std::vector<const char*> GetRequiredSwapchain();

	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue queue = nullptr;
	uint32_t queueIndex = ~0;
};
