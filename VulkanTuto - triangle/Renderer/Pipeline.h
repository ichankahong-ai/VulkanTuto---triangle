#pragma once

#include "Constants.h"

class VulkanDevice;

class Pipeline
{
public:
	void create(VulkanDevice& dev, vk::Format colorFormat, vk::raii::DescriptorSetLayout& descriptorSetLayout);

	vk::raii::Pipeline& getPipeline() { return graphicsPipeline; }
	vk::raii::PipelineLayout& getLayout() { return pipelineLayout; }

private:
	[[nodiscard]] vk::raii::ShaderModule createShaderModule(vk::raii::Device& device, const std::vector<char>& code) const;

	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
};
