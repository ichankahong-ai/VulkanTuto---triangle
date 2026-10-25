#pragma once

#include "Constants.h"

class VulkanDevice;

class Swapchain
{
public:
	void create(VulkanDevice& dev, vk::raii::SurfaceKHR& surface, GLFWwindow* window);
	void createImageViews(VulkanDevice& dev);
	void cleanup();

	vk::raii::SwapchainKHR& get() { return swapChain; }
	std::vector<vk::Image>& getImages() { return swapChainImages; }
	std::vector<vk::raii::ImageView>& getImageViews() { return swapChainImageViews; }
	vk::Format getFormat() const { return swapChainSurfaceFormat.format; }
	vk::Extent2D getExtent() const { return swapChainExtent; }

private:
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities, GLFWwindow* window);

	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> swapChainImageViews;
	vk::SurfaceFormatKHR swapChainSurfaceFormat;
	vk::Extent2D swapChainExtent;
};
