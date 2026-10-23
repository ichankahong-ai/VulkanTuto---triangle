#pragma once

#include "includevk.h"
#include "IncludeG.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static vk::VertexInputBindingDescription getBindingDescription()
	{
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		return {
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
			vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)),
			vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord))
		};
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
};

//    float3(0.157, 1.0, 0.522),
//    float3(0.408, 0.03, 0.03),
//    float3(0.408, 0.03, 0.03)

class Renderer
{
public:

	void createInstance();
	void SetupDebugCallback();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createCommandPool();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers();
	void createSyncObjects();

	void recordCommandBuffer(uint32_t imageIndex);
	void transition_image_layout(
		uint32_t                imageIndex,
		vk::ImageLayout         old_layout,
		vk::ImageLayout         new_layout,
		vk::AccessFlags2        src_access_mask,
		vk::AccessFlags2        dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask);

	void setWindow(GLFWwindow* _window);
	void drawFrame();

	void recreateSwapChain();
	void cleanupSwapChain();

	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);

	void updateUniformBuffer(uint32_t currentImage);
	void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);

	vk::raii::CommandBuffer beginSingleTimeCommands();
	vk::raii::ImageView createImageView(vk::Image const& image, vk::Format format);
	void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer);

	bool framebufferResized = false;
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	vk::raii::Device device = nullptr;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Queue Queue = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;
	vk::raii::Image        textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
	vk::raii::ImageView textureImageView = nullptr;
	vk::raii::Sampler      textureSampler = nullptr;
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;
	vk::raii::Buffer       indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr;
	vk::raii::DescriptorPool descriptorPool = nullptr;
	vk::raii::SwapchainKHR swapChain = nullptr;

	std::vector<vk::raii::DescriptorSet> descriptorSets;

	std::vector<vk::raii::Buffer>       uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*>                 uniformBuffersMapped;

	std::vector<vk::raii::CommandBuffer> commandBuffers;

	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;

	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> swapChainImageViews;
	vk::SurfaceFormatKHR   swapChainSurfaceFormat;
	vk::Extent2D           swapChainExtent;

	uint32_t queueIndex = ~0;

	uint32_t frameIndex = 0;


	GLFWwindow* window = nullptr;

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const
	{
		vk::ShaderModuleCreateInfo createInfo{ .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data()) };
		vk::raii::ShaderModule shaderModule{ device, createInfo };
		return shaderModule;
	}

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities);

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);

	std::vector<const char*> GetRequiredExtension();
	std::vector<const char*> GetRequiredLayers();
	std::vector<const char*> GetRequiredSwapchain();

	

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
};

