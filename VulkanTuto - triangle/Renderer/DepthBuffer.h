#pragma once

#include "Constants.h"

class VulkanDevice;

class DepthBuffer
{
public:
	void create(VulkanDevice& dev, vk::Extent2D extent);

	vk::raii::Image& getImage() { return depthImage; }
	vk::raii::ImageView& getImageView() { return depthImageView; }

private:
	vk::raii::Image depthImage = nullptr;
	vk::raii::DeviceMemory depthImageMemory = nullptr;
	vk::raii::ImageView depthImageView = nullptr;
};
