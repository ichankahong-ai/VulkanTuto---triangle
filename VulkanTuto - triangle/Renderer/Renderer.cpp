#include "Renderer.h"
#include "Mesh.h"

void Renderer::setWindow(GLFWwindow* _window)
{
	window = _window;
}

void Renderer::init()
{
	// Instance, debug, surface
	vulkanInstance.createInstance();
	vulkanInstance.setupDebugCallback();
	vulkanInstance.createSurface(window);

	// GPU et device logique
	vulkanDevice.pickPhysicalDevice(vulkanInstance.getInstance());
	vulkanDevice.createLogicalDevice(vulkanInstance.getSurface());

	// Swapchain
	swapchain.create(vulkanDevice, vulkanInstance.getSurface(), window);
	swapchain.createImageViews(vulkanDevice);

	// Descripteurs et pipeline
	descriptors.createSetLayout(vulkanDevice);
	pipeline.create(vulkanDevice, swapchain.getFormat(), descriptors.getLayout());

	// Commandes, profondeur, texture
	commands.createCommandPool(vulkanDevice);
	depthBuffer.create(vulkanDevice, swapchain.getExtent());
	texture.createTextureImage(vulkanDevice, commands);
	texture.createTextureImageView(vulkanDevice);
	texture.createTextureSampler(vulkanDevice);

	// Buffers
	buffers.createVertexBuffer(vulkanDevice, commands);
	buffers.createIndexBuffer(vulkanDevice, commands);
	buffers.createUniformBuffers(vulkanDevice);

	// Descripteurs (pool + sets) 
	descriptors.createPool(vulkanDevice);
	descriptors.createSets(vulkanDevice, buffers.getUniformBuffers(), texture.getImageView(), texture.getSampler());

	// Command buffers et synchronisation
	commands.createCommandBuffers(vulkanDevice);
	syncObjects.create(vulkanDevice, swapchain.getImages().size());
}

void Renderer::waitIdle()
{
	vulkanDevice.getDevice().waitIdle();
}

void Renderer::recordCommandBuffer(uint32_t imageIndex)
{
	auto& commandBuffer = commands.get(frameIndex);

	commandBuffer.begin({});

	Commands::transition_image_layout(commandBuffer,
		swapchain.getImages()[imageIndex],
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},                                                        // srcAccessMask (no need to wait for previous operations)
		vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,         // dstStage
		vk::ImageAspectFlagBits::eColor
	);

	// Transition depth image to depth attachment optimal layout
	Commands::transition_image_layout(
		commandBuffer,
		*depthBuffer.getImage(),
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthAttachmentOptimal,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::ImageAspectFlagBits::eDepth);

	vk::Extent2D swapChainExtent = swapchain.getExtent();

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);
	vk::RenderingAttachmentInfo attachmentInfo = {
		.imageView = swapchain.getImageViews()[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor };

	vk::RenderingAttachmentInfo depthAttachmentInfo = {
	.imageView = depthBuffer.getImageView(),
	.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
	.loadOp = vk::AttachmentLoadOp::eClear,
	.storeOp = vk::AttachmentStoreOp::eDontCare,
	.clearValue = clearDepth };

	vk::RenderingInfo renderingInfo = {
	.renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent },
	.layerCount = 1,
	.colorAttachmentCount = 1,
	.pColorAttachments = &attachmentInfo,
	.pDepthAttachment = &depthAttachmentInfo };

	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.getPipeline());

	commandBuffer.setViewport(0, vk::Viewport(0.0f, static_cast<float>(swapChainExtent.height), static_cast<float>(swapChainExtent.width), -static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));
	commandBuffer.bindVertexBuffers(0, *buffers.getVertexBuffer(), { 0 });
	commandBuffer.bindIndexBuffer(*buffers.getIndexBuffer(), 0, vk::IndexTypeValue<decltype(indices)::value_type>::value);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.getLayout(), 0, *descriptors.getSet(frameIndex), nullptr);
	commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	commandBuffer.endRendering();

	Commands::transition_image_layout(
		commandBuffer,
		swapchain.getImages()[imageIndex],
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
		{},                                                     // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
		vk::PipelineStageFlagBits2::eBottomOfPipe,             // dstStage
		vk::ImageAspectFlagBits::eColor
	);

	commandBuffer.end();
}

void Renderer::drawFrame()
{
	auto& device = vulkanDevice.getDevice();
	auto& Queue = vulkanDevice.getQueue();
	auto& swapChain = swapchain.get();
	auto& commandBuffers = commands.getCommandBuffers();
	auto& presentCompleteSemaphores = syncObjects.getPresentCompleteSemaphores();
	auto& renderFinishedSemaphores = syncObjects.getRenderFinishedSemaphores();
	auto& inFlightFences = syncObjects.getInFlightFences();

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
	updateUniformBuffer(frameIndex);

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
									  .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex] };

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
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vulkanDevice.getDevice().waitIdle();

	cleanupSwapChain();
	swapchain.create(vulkanDevice, vulkanInstance.getSurface(), window);
	swapchain.createImageViews(vulkanDevice);
	depthBuffer.create(vulkanDevice, swapchain.getExtent());
}

void Renderer::cleanupSwapChain()
{
	swapchain.cleanup();
}

void Renderer::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	vk::Extent2D swapChainExtent = swapchain.getExtent();

	UniformBufferObject ubo{};

	ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj =
		glm::perspective(glm::radians(45.0f), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), 0.1f, 10.0f);

	buffers.updateUniformBuffer(currentImage, &ubo, sizeof(ubo));
}

void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}
