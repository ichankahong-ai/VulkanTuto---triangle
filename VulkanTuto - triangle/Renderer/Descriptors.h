#pragma once

#include "Constants.h"

class VulkanDevice;

class Descriptors
{
public:
	void createSetLayout(VulkanDevice& dev);
	void createPool(VulkanDevice& dev);
	void createSets(VulkanDevice& dev,
		std::vector<vk::raii::Buffer>& uniformBuffers,
		vk::raii::ImageView& textureImageView,
		vk::raii::Sampler& textureSampler);

	vk::raii::DescriptorSetLayout& getLayout() { return descriptorSetLayout; }
	vk::raii::DescriptorSet& getSet(uint32_t frameIndex) { return descriptorSets[frameIndex]; }

private:
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	vk::raii::DescriptorPool descriptorPool = nullptr;
	std::vector<vk::raii::DescriptorSet> descriptorSets;
};
