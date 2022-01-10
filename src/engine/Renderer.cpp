#include "engine/Renderer.hpp"

// std
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>
#include <cmath>

namespace vk_engine{
	Renderer::Renderer(LogicalDevice &device, CommandPool &commandPool) : device{device}, commandPool{commandPool}{
		recreateSwapChain();
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		viewport.width = device.getInstance().getWindow().getExtent().width;
		viewport.height = device.getInstance().getWindow().getExtent().height;

		scissor.offset = {0, 0};
		scissor.extent = device.getInstance().getWindow().getExtent();
	}

	Renderer::~Renderer(){
		freeCommandBuffers();
	}

	void Renderer::build(){
		swapChain->build();
		createCommandBuffers();
	}

	void Renderer::createCommandBuffers(){
		commandBuffers.resize(swapChain->getFramesInFlight());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate command buffers");
	}

	void Renderer::freeCommandBuffers(){
		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void Renderer::recreateSwapChain(){
		VkExtent2D extent = device.getInstance().getWindow().getExtent();

		while(extent.width == 0 | extent.height == 0){
			extent = device.getInstance().getWindow().getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);

		if (swapChain == nullptr){
			swapChain = std::make_unique<SwapChain>(device, extent);
		} else {
			std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
			swapChain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

			// set properties
			swapChain->setRefreshType(oldSwapChain->getRefreshType());
			swapChain->setSurfaceFormat(oldSwapChain->getWantedFormat());
			swapChain->build();

			if (!oldSwapChain->compareSwapFormats(*swapChain.get()))
				throw std::runtime_error("swap chain image or depth format has changed");
			
			if (autoUpdateViewportSize){
				viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
				viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
				scissor.extent = swapChain->getSwapChainExtent();
			}
		}
	}

	VkCommandBuffer Renderer::beginFrame(){
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");

		auto result = swapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		return commandBuffer;
	}

	void Renderer::endFrame(){
		assert(isFrameStarted && "can't call endFrame while frame isn't in progress");
		VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");
		
		VkResult result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || device.getInstance().getWindow().resized()){
			device.getInstance().getWindow().resetWindowResizedFlag();
			recreateSwapChain();
		}

		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to submit the command buffer");
		
		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % swapChain->getFramesInFlight();
	}

	void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame isn't in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");
		
		VkRenderPassBeginInfo renderPassBeginInfo{};

		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapChain->getRenderPass();
		renderPassBeginInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = clearColor;
		clearValues[1].depthStencil = {1.f, 0};

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame isn't in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}
	
	void Renderer::setClearColor(const float &r, const float &g, const float &b, const float &a) noexcept{
		clearColor.float32[0] = r;
		clearColor.float32[1] = g;
		clearColor.float32[2] = b;
		clearColor.float32[3] = a;
	}
	
}