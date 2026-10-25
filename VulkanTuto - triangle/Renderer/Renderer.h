#pragma once

#include "Constants.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "Swapchain.h"
#include "Descriptors.h"
#include "Pipeline.h"
#include "Commands.h"
#include "DepthBuffer.h"
#include "Texture.h"
#include "Buffers.h"
#include "SyncObjects.h"

class Renderer
{
public:
	void setWindow(GLFWwindow* _window);
	void init();          
	void drawFrame();
	void waitIdle();

	void recreateSwapChain();
	void cleanupSwapChain();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	bool framebufferResized = false;

private:
	void recordCommandBuffer(uint32_t imageIndex);
	void updateUniformBuffer(uint32_t currentImage);

	GLFWwindow* window = nullptr;
	uint32_t frameIndex = 0;

	VulkanInstance vulkanInstance;
	VulkanDevice   vulkanDevice;
	Swapchain      swapchain;
	Descriptors    descriptors;
	Pipeline       pipeline;
	Commands       commands;
	DepthBuffer    depthBuffer;
	Texture        texture;
	Buffers        buffers;
	SyncObjects    syncObjects;
};
