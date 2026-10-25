#pragma once

#include "Constants.h"

class VulkanDevice;
class Commands;

class Texture
{
public:
	void createTextureImage(VulkanDevice& dev, Commands& cmds);
	void createTextureImageView(VulkanDevice& dev);
	void createTextureSampler(VulkanDevice& dev);

	vk::raii::ImageView& getImageView() { return textureImageView; }
	vk::raii::Sampler& getSampler() { return textureSampler; }

private:
	void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);

	vk::raii::Image textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
	vk::raii::ImageView textureImageView = nullptr;
	vk::raii::Sampler textureSampler = nullptr;
};
