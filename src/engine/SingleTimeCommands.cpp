#include "engine/SingleTimeCommands.hpp"

namespace vk_engine{
	SingleTimeCommands::SingleTimeCommands(CommandPool &commandPool, LogicalDevice &device, VkQueue &queue) : commandPool{commandPool}, device{device}, queue{queue}{
		begin();
	}

	SingleTimeCommands::~SingleTimeCommands(){
		end();
	}

	void SingleTimeCommands::begin(){
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
	}

	void SingleTimeCommands::end(){
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	void copyBuffer(CommandPool &commandPool, LogicalDevice &device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size){
		SingleTimeCommands commandBuffer(commandPool, device, device.getQueues()[0][FAMILY_GRAPHIC]);

		VkBufferCopy copyRegion{};
		copyRegion.dstOffset = 0;
		copyRegion.srcOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	}

	void copyBufferToImage(CommandPool &commandPool, LogicalDevice &device, VkBuffer buffer, VkImage image, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageLayerCount){
		SingleTimeCommands commandBuffer(commandPool, device, device.getQueues()[0][FAMILY_GRAPHIC]);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = imageLayerCount;

		region.imageOffset = {0, 0, 0};
		region.imageExtent = {imageWidth, imageHeight, 1};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

}