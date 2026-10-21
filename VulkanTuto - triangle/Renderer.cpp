#include <iostream>
#include <map>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <fstream>
#include "Debug.h"
#include "Renderer.h"

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

void Renderer::createInstance()
{

	constexpr vk::ApplicationInfo application_info
	{
		.pApplicationName = "YAY SPIN",
		.applicationVersion = VK_MAKE_VERSION(1,0,0),
		.pEngineName = "Gaming campus vulkan",
		.engineVersion = VK_MAKE_VERSION(1,0,0),
		.apiVersion = vk::ApiVersion14
	};

	std::vector requiredExtensions = GetRequiredExtension();

	auto extensionProperties = context.enumerateInstanceExtensionProperties();

	for (uint32_t i = 0; i < requiredExtensions.size(); i++)
	{
		auto compareExtensions = [glfwExtension = requiredExtensions[i]](auto const& extensionProperty)

			{
				return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
			};

		if (std::ranges::none_of(extensionProperties, compareExtensions))
		{
			throw std::runtime_error("required GLFW extension not supported: " + std::string(requiredExtensions[i]));

		}
	}

	std::vector<char const*> requiredLayers = GetRequiredLayers();

	auto layerProperties = context.enumerateInstanceLayerProperties();

	auto unsupportedLayerIt = std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer)
	{
		return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
			{
				return strcmp(layerProperty.layerName, requiredLayer) == 0;
			});
	});

	if (unsupportedLayerIt != requiredLayers.end())
	{
		throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
	}

	vk::InstanceCreateInfo create_info
	{
		.pApplicationInfo = &application_info,
		.enabledLayerCount = static_cast<uint32_t> (requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t> (requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data()

	};

	instance = vk::raii::Instance(context, create_info);

	SetupDebugCallback();
}

void Renderer::SetupDebugCallback()
{
	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	vk::DebugUtilsMessengerCreateInfoEXT debugInfo{ .messageSeverity = severityFlags, .messageType = messageTypeFlags, .pfnUserCallback = &DebugCallBack };

	debugMessenger = instance.createDebugUtilsMessengerEXT(debugInfo);
}

vk::Bool32 Renderer::DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cout << "Validation layer: Type [" << vk::to_string(type) << "] - Message : " << pCallbackData->pMessage << std::endl;

	std::println(std::cout, "Validation layer: Type [{}]", to_string(type));
	std::println(std::cout, "Message : {}", pCallbackData->pMessage);

	return vk::False;
}

void Renderer::createSurface()
{
	VkSurfaceKHR	_surface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
		throw std::runtime_error("failed to create window surface!");
	}
	surface = vk::raii::SurfaceKHR(instance, _surface);
}

bool Renderer::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
	// Check if the physicalDevice supports the Vulkan 1.3 API version
	bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

	// Check if any of the queue families support graphics operations
	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp) { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

	auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequiredExtensions =
		std::ranges::all_of(GetRequiredSwapchain(),
			[&availableDeviceExtensions](auto const& GetRequiredSwapchain)
			{
				return std::ranges::any_of(availableDeviceExtensions,
					[GetRequiredSwapchain](auto const& availableDeviceExtension)
					{ return strcmp(availableDeviceExtension.extensionName, GetRequiredSwapchain) == 0; });
			});

	// Check if the physicalDevice supports the required features (shader draw parameters, dynamic rendering and extended dynamic state)
	auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
	bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

	// Return true if the physicalDevice meets all the criteria
	return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

void Renderer::pickPhysicalDevice()
{
	std::vector<vk::raii::PhysicalDevice> PhysicalDevices = instance.enumeratePhysicalDevices();
	auto const devIter = std::ranges::find_if(PhysicalDevices, [&](auto const& physicalDevice) { return isDeviceSuitable(physicalDevice); });
	if (devIter == PhysicalDevices.end())
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	physicalDevice = *devIter;
}

void Renderer::createLogicalDevice()
{
	// find the index of the first queue family that supports graphics
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
	{
		if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
		{
			// found a queue family that supports both graphics and present
			queueIndex = qfpIndex;
			break;
		}
	}
	if (queueIndex == ~0)
	{
		throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
	}

	// query for Vulkan 1.3 features
	vk::StructureChain<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan11Features,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
		featureChain = {
			{},                                    // vk::PhysicalDeviceFeatures2
			{.shaderDrawParameters = true},        // vk::PhysicalDeviceVulkan11Features
			{.dynamicRendering = true},            // vk::PhysicalDeviceVulkan13Features
			{.extendedDynamicState = true}         // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
	};

	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

	// create a Device
	float                     queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
	vk::DeviceCreateInfo      deviceCreateInfo{ .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
											   .queueCreateInfoCount = 1,
											   .pQueueCreateInfos = &deviceQueueCreateInfo,
											   .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
											   .ppEnabledExtensionNames = requiredDeviceExtension.data()};

	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	Queue = vk::raii::Queue(device, queueIndex, 0);
}



vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
{
	const auto formatIt = std::ranges::find_if(
		availableFormats, [](const auto& format) {return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;}
	);
	return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR Renderer::chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
	assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {return presentMode == vk::PresentModeKHR::eFifo;}));
	return std::ranges::any_of(availablePresentModes,
		[](const vk::PresentModeKHR value) {return vk::PresentModeKHR::eMailbox == value;}) ?
		vk::PresentModeKHR::eMailbox :
		vk::PresentModeKHR::eFifo;
}

vk::Extent2D Renderer::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	return {
		std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	};
}

uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
	{
		minImageCount = surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

void Renderer::createSwapChain()
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
	swapChainExtent = chooseSwapExtent(surfaceCapabilities);
	uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

	std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
	swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

	std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
	

	vk::SwapchainCreateInfoKHR swapChainCreateInfo{
		.surface = *surface,
		.minImageCount = minImageCount,
		.imageFormat = swapChainSurfaceFormat.format,
		.imageColorSpace = swapChainSurfaceFormat.colorSpace,
		.imageExtent = swapChainExtent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = chooseSwapPresentMode(availablePresentModes),
		.clipped = true
	};

	swapChainCreateInfo.oldSwapchain = nullptr;

	swapChain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
	swapChainImages = swapChain.getImages();
}

void Renderer::createImageViews()
{
	assert(swapChainImageViews.empty());

	vk::ImageViewCreateInfo imageViewCreateInfo{
		.viewType = vk::ImageViewType::e2D,
		.format = swapChainSurfaceFormat.format,
		.subresourceRange ={vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
	};

	imageViewCreateInfo.components = {
		vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity
	};

	imageViewCreateInfo.subresourceRange = {
		.aspectMask = vk::ImageAspectFlagBits::eColor,
		.levelCount = 1,
		.layerCount = 1
	};

	for (auto& image : swapChainImages) {
		imageViewCreateInfo.image = image;
		swapChainImageViews.emplace_back(device, imageViewCreateInfo);
	}
}

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	std::vector<char> buffer(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	file.close();

	return buffer;
}

void Renderer::createGraphicsPipeline()
{
	/*auto shaderCode = readFile("shaders/slang.spv");*/
	vk::raii::ShaderModule shaderModule = createShaderModule(readFile("shaders/slang.spv"));

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule,  .pName = "vertMain" };
	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };
	
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };


	vk::Viewport viewport{ 0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f };
	vk::Rect2D scissor{ vk::Offset2D{ 0, 0 }, swapChainExtent };
	vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };

	vk::PipelineRasterizationStateCreateInfo rasterizer{ .depthClampEnable = vk::False,
													.rasterizerDiscardEnable = vk::False,
													.polygonMode = vk::PolygonMode::eFill,
													.cullMode = vk::CullModeFlagBits::eBack,
													.frontFace = vk::FrontFace::eClockwise,
													.depthBiasEnable = vk::False,
													.lineWidth = 1.0f };

	vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };

	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
	.blendEnable = vk::True,
	.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
	.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
	.colorBlendOp = vk::BlendOp::eAdd,
	.srcAlphaBlendFactor = vk::BlendFactor::eOne,
	.dstAlphaBlendFactor = vk::BlendFactor::eZero,
	.alphaBlendOp = vk::BlendOp::eAdd,
	.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};

	vk::PipelineColorBlendStateCreateInfo colorBlending{
	.logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colorBlendAttachment };



	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 0, .pushConstantRangeCount = 0 };

	pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);


	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{ .colorAttachmentCount = 1, .pColorAttachmentFormats = &swapChainSurfaceFormat.format };

	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
	{.stageCount = 2,
	 .pStages = shaderStages,
	 .pVertexInputState = &vertexInputInfo,
	 .pInputAssemblyState = &inputAssembly,
	 .pViewportState = &viewportState,
	 .pRasterizationState = &rasterizer,
	 .pMultisampleState = &multisampling,
	 .pColorBlendState = &colorBlending,
	 .pDynamicState = &dynamicState,
	 .layout = pipelineLayout,
	 .renderPass = nullptr},
	{.colorAttachmentCount = 1, .pColorAttachmentFormats = &swapChainSurfaceFormat.format}
	};

	graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}

void Renderer::createCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
								   .queueFamilyIndex = queueIndex };

	commandPool = vk::raii::CommandPool(device, poolInfo);
}

void Renderer::createCommandBuffers()
{
	/*vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };

	commandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front());*/

	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = MAX_FRAMES_IN_FLIGHT };

	commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
}

void Renderer::createSyncObjects()
{
	/*presentCompleteSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());
	renderFinishedSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());
	drawFence = vk::raii::Fence(device, {.flags = vk::FenceCreateFlagBits::eSignaled});*/
	assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
		inFlightFences.emplace_back(device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
	}

}

void Renderer::recordCommandBuffer(uint32_t imageIndex)
{
	auto& commandBuffer = commandBuffers[frameIndex];

	commandBuffer.begin({});

	transition_image_layout(imageIndex,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},                                                        // srcAccessMask (no need to wait for previous operations)
		vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
		vk::PipelineStageFlagBits2::eColorAttachmentOutput         // dstStage
	);

	vk::ClearValue              clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::RenderingAttachmentInfo attachmentInfo = {
		.imageView = swapChainImageViews[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor };

	vk::RenderingInfo renderingInfo = {
	.renderArea = {.offset = {0, 0}, .extent = swapChainExtent},
	.layerCount = 1,
	.colorAttachmentCount = 1,
	.pColorAttachments = &attachmentInfo };

	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);

	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));

	commandBuffer.draw(3, 1, 0, 0);

	commandBuffer.endRendering();

	transition_image_layout(
		imageIndex,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
		{},                                                     // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
		vk::PipelineStageFlagBits2::eBottomOfPipe               // dstStage
	);

	commandBuffer.end();
}

void Renderer::transition_image_layout(uint32_t imageIndex,
	vk::ImageLayout old_layout,
	vk::ImageLayout new_layout,
	vk::AccessFlags2 src_access_mask,
	vk::AccessFlags2 dst_access_mask,
	vk::PipelineStageFlags2 src_stage_mask,
	vk::PipelineStageFlags2 dst_stage_mask)
{
	vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = src_stage_mask,
			.srcAccessMask = src_access_mask,
			.dstStageMask = dst_stage_mask,
			.dstAccessMask = dst_access_mask,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = swapChainImages[imageIndex],
			.subresourceRange = {
				   .aspectMask = vk::ImageAspectFlagBits::eColor,
				   .baseMipLevel = 0,
				   .levelCount = 1,
				   .baseArrayLayer = 0,
				   .layerCount = 1} };
	vk::DependencyInfo dependency_info = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier };

	commandBuffers[frameIndex].pipelineBarrier2(dependency_info);
}

void Renderer::setWindow(GLFWwindow* _window)
{
	window = _window;
}

void Renderer::drawFrame()
{
	auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}
	
	auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized)
	{
		framebufferResized = false;
		recreateSwapChain();
		/*return;*/
	}

	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	device.resetFences(*inFlightFences[frameIndex]);

	commandBuffers[frameIndex].reset();
	recordCommandBuffer(imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo   submitInfo{ .waitSemaphoreCount = 1,
									  .pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
									  .pWaitDstStageMask = &waitDestinationStageMask,
									  .commandBufferCount = 1,
									  .pCommandBuffers = &*commandBuffers[frameIndex],
									  .signalSemaphoreCount = 1,
									  .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex]};

	Queue.submit(submitInfo, *inFlightFences[frameIndex]);

	const vk::PresentInfoKHR presentInfoKHR{
	.waitSemaphoreCount = 1,
	.pWaitSemaphores = &*renderFinishedSemaphores[frameIndex],
	.swapchainCount = 1,
	.pSwapchains = &*swapChain,
	.pImageIndices = &imageIndex };

	/*presentInfoKHR.pResults = nullptr;*/
	result = Queue.presentKHR(presentInfoKHR);

	if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR))
	{
		recreateSwapChain();
	}
	else
	{
		assert(result == vk::Result::eSuccess);
	}

	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while ((width == 0 || height == 0) && !glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	if (glfwWindowShouldClose(window)) {
		return;
	}

	device.waitIdle();

	cleanupSwapChain();
	createSwapChain();
	createImageViews();
}

void Renderer::cleanupSwapChain()
{
	swapChainImageViews.clear();
	swapChain = nullptr;
}
 
void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}

std::vector<const char*> Renderer::GetRequiredExtension()
{

	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<char const*> requiredExtensions = { vk::EXTDebugUtilsExtensionName };

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	extensions.append_range(requiredExtensions);

	return extensions;
}

std::vector<const char*> Renderer::GetRequiredLayers()
{
	const std::vector ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	std::vector<char const*> requiredLayers = {};

	if (Debug::enableDebug)
		requiredLayers.append_range(ValidationLayers);

	return requiredLayers;
}

std::vector<const char*> Renderer::GetRequiredSwapchain()
{
	/*uint32_t glfwExtensionCount2 = 0;
	auto glfwExtensions2 = glfwGetRequiredInstanceExtensions(&glfwExtensionCount2);

	std::vector<char const*> requiredExtensions2 = { vk::KHRSwapchainExtensionName };

	std::vector extensions2(glfwExtensions2, glfwExtensions2 + glfwExtensionCount2);

	extensions2.append_range(requiredExtensions2);

	return extensions2;*/

	std::vector<char const*> requiredExtensions2 = { vk::KHRSwapchainExtensionName };
	return requiredExtensions2;
}

