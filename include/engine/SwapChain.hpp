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