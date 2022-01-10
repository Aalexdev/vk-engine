#pragma once

#include "engine/LogicalDevice.hpp"
#include "engine/SwapChain.hpp"
#include "engine/CommandPool.hpp"

// libs
#include <vulkan/vulkan.hpp>

// std
#include <vector>

namespace vk_engine{
	class Renderer{
		public:
			Renderer(LogicalDevice &device, CommandPool &commandPool);
			~Renderer();
			
			Renderer(const Renderer&) = delete;
			Renderer& operator=(const Renderer&) = delete;
			
			/**
			 * @brief get the render pass of the swap chain
			 * @return VkRenderPass 
			 */
			VkRenderPass getSwapChainRenderPass() const {return swapChain->getRenderPass();}

			/**
			 * @brief get if a frame is in progress
			 * @return true if in protgress, false if not
			 */
			bool isFrameInProgress() const {return isFrameStarted;}

			/**
			 * @brief get the current frame buffer
			 * @return VkCommandBuffer 
			 */
			VkCommandBuffer getCurrentCommandBuffer() const {
				assert(isFrameStarted && "Cannot get command buffer whene frame not in progress");
				return commandBuffers[currentFrameIndex];
			};

			/**
			 * @brief get the current frame index
			 * @return int 
			 */
			int getFrameIndex() const {
				assert(isFrameStarted && "Cannot get frame index whene frame not in progress");
				return currentFrameIndex;
			}

			/**
			 * @brief get a reference to the swapChain to chang attributes before the build
			 * @return SwapChain& 
			 */
			SwapChain &getSwapChain() noexcept {return *swapChain.get();}

			/**
			 * @brief construct the renderer
			 */
			void build();

			/**
			 * @brief begin a frame and return the linked command Buffer
			 * @return VkCommandBuffer 
			 */
			VkCommandBuffer beginFrame();

			/**
			 * @brief end the begined frame
			 */
			void endFrame();

			/**
			 * @brief begin the renderPass
			 * @param commandBuffer 
			 */
			void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

			/**
			 * @brief end the renderPass
			 * @param commandBuffer 
			 */
			void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

			/**
			 * @brief set the refresh color
			 * 
			 * @param r the red channel [0,1]
			 * @param g the green channel [0,1]
			 * @param b the blue channel [0,1]
			 * @param a the alpha channel [0,1]
			 */
			void setClearColor(const float &r = 0.f, const float &g = 0.f, const float &b = 0.f, const float &a = 0.f) noexcept;

			/**
			 * @brief set the position of the viewport
			 * @param x the position on the x axis in pixels
			 * @param y the position on the y axis in pixels
			 */
			void setViewPortPosition(const float &x = 0.f, const float &y = 0.f) noexcept {viewport.x = x; viewport.y = y;}

			/**
			 * @brief set the size of the wiewport
			 * 
			 * @param width the width in pixels
			 * @param height the height in pixels
			 */
			void setViewPortSize(const float &width, const float &height) noexcept {viewport.width = width; viewport.height = height;}

			/**
			 * @brief set the automatic update of the viewoprt on swapchain resize, and so on window resizing
			 * @param autoUpdate auto resizing the viewport to the window
			 */
			void setAutoUpdateViewportSize(bool autoUpdate) noexcept {autoUpdateViewportSize = autoUpdate;}
		
		private:
			void createCommandBuffers();
			void freeCommandBuffers();
			void recreateSwapChain();
			
			LogicalDevice &device;
			CommandPool &commandPool;

			std::unique_ptr<SwapChain> swapChain;
			std::vector<VkCommandBuffer> commandBuffers{};

			uint32_t currentImageIndex = 0;
			int currentFrameIndex = 0;
			bool isFrameStarted = false;

			VkClearColorValue clearColor = {0.f, 0.f, 0.f, 0.f};
			VkViewport viewport;
			VkRect2D scissor;
			bool autoUpdateViewportSize = true;
			
	};
}