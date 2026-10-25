#pragma once

#include "Constants.h"

class VulkanDevice;

class Commands
{
public:
	void createCommandPool(VulkanDevice& dev);
	void createCommandBuffers(VulkanDevice& dev);

	vk::raii::CommandBuffer beginSingleTimeCommands(VulkanDevice& dev);
	void endSingleTimeCommands(VulkanDevice& dev, vk::raii::CommandBuffer&& commandBuffer);

	static void transition_image_layout(
		vk::raii::CommandBuffer& commandBuffer,
		vk::Image               image,
		vk::ImageLayout         old_layout,
		vk::ImageLayout         new_layout,
		vk::AccessFlags2        src_access_mask,
		vk::AccessFlags2        dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask,
		vk::ImageAspectFlags    image_aspect_flags);

	vk::raii::CommandBuffer& get(uint32_t frameIndex) { return commandBuffers[frameIndex]; }
	std::vector<vk::raii::CommandBuffer>& getCommandBuffers() { return commandBuffers; }

private:
	vk::raii::CommandPool commandPool = nullptr;
	std::vector<vk::raii::CommandBuffer> commandBuffers;
};
