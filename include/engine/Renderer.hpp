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
	};
}