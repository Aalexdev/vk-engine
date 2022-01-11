#pragma once

#include "engine/LogicalDevice.hpp"

// libs
#include <vulkan/vulkan.h>

namespace vk_engine{
	class Buffer{
		public:
			Buffer(LogicalDevice &device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags,VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
			~Buffer();

			// avoid copy
			Buffer(const Buffer &) = delete;
			Buffer &operator=(const Buffer &) = delete;

			/**
			 * @brief map the memory of the buffer
			 * 
			 * @param size the size of the buffer to map
			 * @param offset the offset of the beffer's part to map
			 * @return VkResult 
			 */
			VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

			/**
			 * @brief unmap the buffer
			 */
			void unmap();

			/**
			 * @brief write the data to the buffer
			 * 
			 * @param data the data to write
			 * @param size the size of the buffer to write
			 * @param offset the offset the given size in the buffer
			 */
			void write(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

			/**
			 * @brief update the buffer to the V-RAM
			 * 
			 * @param size the size of the buffer to update
			 * @param offset the offset in the buffer of the part of the memory to update
			 * @return VkResult 
			 */
			VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

			/**
			 * @brief get the descriptor information of the buffer
			 * 
			 * @param size the size of the buffer
			 * @param offset the offset in the buffer
			 * @return VkDescriptorBufferInfo 
			 */
			VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

			/**
			 * @brief invalidate the buffer to make it visibile for the buffer
			 * 
			 * @param size the size of the buffer to invalidate
			 * @param offset the offset of the size
			 * @return VkResult 
			 */
			VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

			/**
			 * @brief write the data at the given index
			 * 
			 * @param data the data to send
			 * @param index the index where to write
			 */
			void writeToIndex(void* data, int index);

			/**
			 * @brief flush to the V-RAM the given index
			 * 
			 * @param index the index to send to the V-RAM
			 * @return VkResult 
			 */
			VkResult flushIndex(int index);

			/**
			 * @brief get the descriptor info of the given index
			 * 
			 * @param index the index of theinstance
			 * @return VkDescriptorBufferInfo 
			 */
			VkDescriptorBufferInfo descriptorInfoForIndex(int index);

			/**
			 * @brief invalidate to the index to make it visible for the host
			 * 
			 * @param index the index of the instance
			 * @return VkResult 
			 */
			VkResult invalidateIndex(int index);

			/**
			 * @brief get the vulkan buffer
			 * @return VkBuffer 
			 */
			VkBuffer getBuffer() const noexcept {return buffer;}

			/**
			 * @brief get the mapped memory (need to map the memory before use)
			 * @return void* 
			 */
			void* getMappedMemory() const noexcept {return mapped;}

			/**
			 * @brief get the count of instance in the buffer
			 * @return uint32_t 
			 */
			uint32_t getInstanceCount() const noexcept {return instanceCount;}

			/**
			 * @brief get the size of one instance
			 * @return VkDeviceSize 
			 */
			VkDeviceSize getInstanceSize() const noexcept {return instanceSize;}

			/**
			 * @brief get the size of the alignement
			 * @return VkDeviceSize 
			 */
			VkDeviceSize getAlignmentSize() const noexcept {return instanceSize;}

			/**
			 * @brief get the usage of the buffer
			 * @return VkBufferUsageFlags 
			 */
			VkBufferUsageFlags getUsageFlags() const noexcept {return usageFlags;}

			/**
			 * @brief get the property of the memory
			 * @return VkMemoryPropertyFlags 
			 */
			VkMemoryPropertyFlags getMemoryPropertyFlags() const noexcept {return memoryPropertyFlags;}

			/**
			 * @brief get the total size of the buffer
			 * @return VkDeviceSize 
			 */
			VkDeviceSize getBufferSize() const noexcept {return bufferSize;}

			// operators
			operator VkBuffer() const noexcept {return buffer;}
		
		private:
			static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

			LogicalDevice &device;

			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			void* mapped = nullptr;

			VkDeviceSize bufferSize;
			uint32_t instanceCount;
			VkDeviceSize instanceSize;
			VkDeviceSize alignmentSize;
			VkBufferUsageFlags usageFlags;
			VkMemoryPropertyFlags memoryPropertyFlags;
	};
}