#pragma once

#include "engine/CommandPool.hpp"
#include "engine/LogicalDevice.hpp"

// libs
#include <vulkan/vulkan.h>

namespace vk_engine{
	class SingleTimeCommands{
		public:
			SingleTimeCommands(CommandPool &commandPool, LogicalDevice &device, VkQueue queue);
			~SingleTimeCommands();

			// avoid copy
			SingleTimeCommands(const SingleTimeCommands &) = delete;
			SingleTimeCommands &operator=(const SingleTimeCommands &) = delete;

			/**
			 * @brief get the single time vulkan command buffer
			 * @return VkCommandBuffer 
			 */
			VkCommandBuffer getCommandBuffer() const noexcept {return commandBuffer;}

			// operators
			operator VkCommandBuffer() const noexcept{return commandBuffer;}
		
		private:
			void begin();
			void end();

			VkCommandBuffer commandBuffer;
			CommandPool &commandPool;
			LogicalDevice &device;
			VkQueue queue;
	};
	
	/**
	 * @brief copy the content of the srcBuffer into the dstBuffer, pick as the queue the first graphic queue of the given device.
	 * 
	 * @param commandPool the reference to a commandPool
	 * @param device a reference to a LogicalDevice instance
	 * @param srcBuffer the buffer to copy
	 * @param dstBuffer the buffer where the src buffer will be coppied
	 * @param size the size of the srcBuffer to copy into the dstBuffer
	 */
	void copyBuffer(CommandPool &commandPool, LogicalDevice &device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	/**
	 * @brief copy the content if the Buffer and copy it into the given image, pick as the queue the fisrt graphic queue of the given logical device.
	 * 
	 * @param commandPool a reference to a commandPool
	 * @param device a reference to a LogicalDevice instance
	 * @param buffer the buffer to copy into the image
	 * @param image the image where the buffer will be coppied
	 * @param imageWidth the width of the image (in pixels)
	 * @param imageHeight the height of teh image (in pixels)
	 * @param imageLayerCount the count of layers of the image
	 */
	void copyBufferToImage(CommandPool &commandPool, LogicalDevice &device, VkBuffer buffer, VkImage image, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageLayerCount);

	/**
	 * @brief make a transition between two layouts
	 * 
	 * @param commandPool a reference to a commandPool
	 * @param device a reference to a LogicalDevice
	 * @param image the image to convert
	 * @param format the format of the image
	 * @param oldLayout the old layout of the image
	 * @param newLayout the new layout of the image
	 */
	void transitionImageLayout(CommandPool &commandPool, LogicalDevice &device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
}