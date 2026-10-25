#include "DepthBuffer.h"
#include "VulkanDevice.h"

void DepthBuffer::create(VulkanDevice& dev, vk::Extent2D extent)
{
	vk::Format depthFormat = dev.findDepthFormat();

	std::tie(depthImage, depthImageMemory) = dev.createImage(extent.width, extent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);
	depthImageView = dev.createImageView(*depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}
