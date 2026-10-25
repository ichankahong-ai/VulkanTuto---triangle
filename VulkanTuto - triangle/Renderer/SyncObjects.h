#pragma once

#include "Constants.h"

class VulkanDevice;

class SyncObjects
{
public:
	void create(VulkanDevice& dev, size_t swapChainImageCount);

	std::vector<vk::raii::Semaphore>& getPresentCompleteSemaphores() { return presentCompleteSemaphores; }
	std::vector<vk::raii::Semaphore>& getRenderFinishedSemaphores() { return renderFinishedSemaphores; }
	std::vector<vk::raii::Fence>& getInFlightFences() { return inFlightFences; }

private:
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;
};
