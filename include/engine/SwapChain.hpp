#pragma once

#include "engine/LogicalDevice.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <memory>
#include <vector>

namespace vk_engine{
	class SwapChain{
		public:
			enum Refresh{
				REFRESH_MAILBOX_MODE = VK_PRESENT_MODE_MAILBOX_KHR,
				REFRESH_IMMEDIATE_MODE = VK_PRESENT_MODE_IMMEDIATE_KHR,
				REFRESH_FIFO_MODE = VK_PRESENT_MODE_FIFO_KHR,
			};

			SwapChain(LogicalDevice &device, VkExtent2D windowExtent);
			SwapChain(LogicalDevice &device, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
			~SwapChain();

			SwapChain(const SwapChain &) = delete;
			SwapChain &operator=(const SwapChain &) = delete;

			/**
			 * @brief set the refresh type of the swap chain.
			 * @param refreshType the refresh type of the swapChain
			 */
			void setRefreshType(Refresh refreshType) {this->refreshType = refreshType;}

			/**
			 * @brief set the wanted surface format of the swap chain, of the wanted format is not available, it's will choose the first available format.
			 * @param format the wanted format
			 */
			void setSurfaceFormat(VkSurfaceFormatKHR format) {wantedSurfaceFormat = format;}

			/**
			 * @brief build the swapChain
			 */
			void build();

			/**
			 * @brief find and return a suitable depth buffer format
			 * @return VkFormat 
			 */
			VkFormat findDepthFormat();

			/**
			 * @brief get the frame buffer at the given index
			 * @param index the index of the frame buffer
			 * @return VkFramebuffer 
			 */
			VkFramebuffer getFrameBuffer(int index) const noexcept {return swapChainFramebuffers[index];}

			/**
			 * @brief get the vulkan render pass
			 * @return VkRenderPass 
			 */
			VkRenderPass getRenderPass() const noexcept {return renderPass;}

			/**
			 * @brief get the image view of the given index
			 * @param index the index of the image
			 * @return VkImageView 
			 */
			VkImageView getImageView(int index) const noexcept {return swapChainImageViews[index];}

			/**
			 * @brief get the count of images
			 * @return size_t 
			 */
			size_t imageCount() const noexcept {return swapChainImages.size();}

			/**
			 * @brief get the format of the swapchain
			 * @return VkFormat 
			 */
			VkFormat getSwapChainImageFormat() const noexcept {return swapChainImageFormat;}

			/**
			 * @brief get the extent of the swap chain
			 * @return VkExtent2D 
			 */
			VkExtent2D getSwapChainExtent() const noexcept {return swapChainExtent;}

			/**
			 * @brief get the width of the swap chain, the width of the swap chain extent
			 * @return uint32_t 
			 */
			uint32_t width() const noexcept {return swapChainExtent.width;}

			/**
			 * @brief get the height of the swap chain, the height of the swap chain extent
			 * @return uint32_t 
			 */
			uint32_t height() const noexcept {return swapChainExtent.height;}

			/**
			 * @brief get the count of frames in flight
			 * @return const int 
			 */
			const int getFramesInFlight() const noexcept {return framesInFlight;}

			/**
			 * @brief get the next image
			 * @param imageIndex a pointer to the imageIndex (must not be null)
			 * @return VkResult 
			 */
			VkResult acquireNextImage(uint32_t *imageIndex);

			/**
			 * @brief submit command buffer
			 * @param buffers the commandBuffer
			 * @param imageIndex the image index
			 * @return VkResult 
			 */
			VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

			/**
			 * @brief compare this swap chain with with the given swap chain
			 * @param swapChain the swap chain to compare
			 * @return true 
			 */
			bool compareSwapFormats(const SwapChain &swapChain) const noexcept { return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat;}

		private:
			void create();
			void createSwapChain();
			void createImageViews();
			void createDepthResources();
			void createRenderPass();
			void createFramebuffers();
			void createSyncObjects();

			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
			VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
			VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

			VkFormat swapChainImageFormat;
			VkFormat swapChainDepthFormat;
			VkExtent2D swapChainExtent;

			std::vector<VkFramebuffer> swapChainFramebuffers;
			VkRenderPass renderPass;

			std::vector<VkImage> depthImages;
			std::vector<VkDeviceMemory> depthImageMemorys;
			std::vector<VkImageView> depthImageViews;
			std::vector<VkImage> swapChainImages;
			std::vector<VkImageView> swapChainImageViews;

			LogicalDevice &device;
			VkExtent2D windowExtent;

			VkSwapchainKHR swapChain;
			std::shared_ptr<SwapChain> oldSwapChain = nullptr;

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;
			std::vector<VkFence> imagesInFlight;
			size_t currentFrame = 0;

			Refresh refreshType = REFRESH_FIFO_MODE;
			VkSurfaceFormatKHR wantedSurfaceFormat = {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

			int framesInFlight = 2;
	};
}