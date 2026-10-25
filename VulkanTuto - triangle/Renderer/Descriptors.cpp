#include "Descriptors.h"
#include "VulkanDevice.h"
#include "Vertex.h"

void Descriptors::createSetLayout(VulkanDevice& dev)
{
	auto& device = dev.getDevice();

	std::array<vk::DescriptorSetLayoutBinding, 2> bindings{
			{ { .binding = 0, .descriptorType = vk::DescriptorType::eUniformBuffer, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eVertex },
			 { .binding = 1, .descriptorType = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eFragment } } };

	vk::DescriptorSetLayoutCreateInfo layoutInfo{ .bindingCount = static_cast<uint32_t>(bindings.size()), .pBindings = bindings.data() };
	descriptorSetLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);
}

void Descriptors::createPool(VulkanDevice& dev)
{
	auto& device = dev.getDevice();

	std::array<vk::DescriptorPoolSize, 2> poolSize{ { { .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = MAX_FRAMES_IN_FLIGHT },
														{ .type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = MAX_FRAMES_IN_FLIGHT } } };
	vk::DescriptorPoolCreateInfo          poolInfo{ .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
												   .maxSets = MAX_FRAMES_IN_FLIGHT,
												   .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
												   .pPoolSizes = poolSize.data() };
	descriptorPool = vk::raii::DescriptorPool(device, poolInfo);
}

void Descriptors::createSets(VulkanDevice& dev,
	std::vector<vk::raii::Buffer>& uniformBuffers,
	vk::raii::ImageView& textureImageView,
	vk::raii::Sampler& textureSampler)
{
	auto& device = dev.getDevice();

	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);
	vk::DescriptorSetAllocateInfo        allocInfo{ .descriptorPool = descriptorPool,
												   .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
												   .pSetLayouts = layouts.data() };

	descriptorSets = device.allocateDescriptorSets(allocInfo);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DescriptorBufferInfo bufferInfo{ .buffer = uniformBuffers[i], .offset = 0, .range = sizeof(UniformBufferObject) };
		vk::DescriptorImageInfo  imageInfo{ .sampler = textureSampler, .imageView = textureImageView, .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal };

		std::array<vk::WriteDescriptorSet, 2>   descriptorWrite{ { { .dstSet = descriptorSets[i],
																 .dstBinding = 0,
																 .dstArrayElement = 0,
																 .descriptorCount = 1,
																 .descriptorType = vk::DescriptorType::eUniformBuffer,
																 .pBufferInfo = &bufferInfo },
																{ .dstSet = descriptorSets[i],
																 .dstBinding = 1,
																 .dstArrayElement = 0,
																 .descriptorCount = 1,
																 .descriptorType = vk::DescriptorType::eCombinedImageSampler,
																 .pImageInfo = &imageInfo } } };
		device.updateDescriptorSets(descriptorWrite, {});
	}
}
