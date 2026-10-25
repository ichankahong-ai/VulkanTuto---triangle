#pragma once

#include "Constants.h"

class VulkanDevice;
class Commands;

class Buffers
{
public:
	void createVertexBuffer(VulkanDevice& dev, Commands& cmds);
	void createIndexBuffer(VulkanDevice& dev, Commands& cmds);
	void createUniformBuffers(VulkanDevice& dev);

	void updateUniformBuffer(uint32_t currentImage, const void* data, size_t size);

	vk::raii::Buffer& getVertexBuffer() { return vertexBuffer; }
	vk::raii::Buffer& getIndexBuffer() { return indexBuffer; }
	std::vector<vk::raii::Buffer>& getUniformBuffers() { return uniformBuffers; }

private:
	void copyBuffer(VulkanDevice& dev, Commands& cmds, vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);

	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;
	vk::raii::Buffer indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr;

	std::vector<vk::raii::Buffer> uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;
};
