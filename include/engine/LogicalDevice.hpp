#pragma once

#include "engine/PhysicalDevice.hpp"
#include "engine/Instance.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <array>
#include <vector>
#include <string>

namespace vk_engine{
	class LogicalDevice{
		public:
			LogicalDevice(Instance &instance, PhysicalDevice &device);
			~LogicalDevice();

			// avoid copy
			LogicalDevice(const LogicalDevice &) = delete;
			LogicalDevice &operator=(LogicalDevice &) = delete;

			/**
			 * @brief require the given extension
			 * @param extension the vulkan extension
			 */
			void requireExtension(const char *extension) {requiredExtensions.push_back(extension);}

			/**
			 * @brief set the priority of the queu used by the logical device
			 * @param priority the priority level
			 */
			void setQueuePriority(const float &priority, const int &queueIndex) {queuePriorities[queueIndex] = priority;}

			/**
			 * @brief set the queue priority of all queues
			 * @param priority the queues priority
			 * @bug not working
			 * TODO : rebuild the function to work
			 */
			void setQueuesPriorities(const float &priority){std::fill(queuePriorities.begin(), queuePriorities.end(), priority);}

			/**
			 * @brief set the count of queues to create
			 * @param count the count of queues
			 */
			void setQueueCount(const int &count) {queuePriorities.reserve(count); queueCount = static_cast<uint32_t>(count);}

			/**
			 * @brief build the logical device from the given parameters
			 */
			void build();

			/**
			 * @brief get the queues
			 * @return std::vector<std::array<VkQueue, PhysicalDevice::FAMILY_TYPE_COUNT>> 
			 */
			std::vector<std::array<VkQueue, FAMILY_TYPE_COUNT>> getQueues() const noexcept {return queues;}

			/**
			 * @brief get the physical device of the logical device
			 * @return PhysicalDevice reference
			 */
			PhysicalDevice& getPhysicalDevice() const noexcept {return physicalDevice;}

			/**
			 * @brief get the reference to the reference
			 * @return Instance& 
			 */
			Instance &getInstance() const noexcept {return instance;}

			/**
			 * @brief create an image from the given informations
			 *
			 * @param imageInfo the information about the image creation 
			 * @param properties the memory properties
			 * @param image the reference to the image
			 * @param imageMemory the reference to the allocated memory
			 */
			void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

			/**
			 * @brief create a buffer from the given informations
			 * 
			 * @param size the size of the buffer
			 * @param usage the usage of the buffer
			 * @param properties the buffer's properties
			 * @param buffer a reference to the buffer
			 * @param bufferMemory a reference to the buffer memory 
			 */
			void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,VkDeviceMemory &bufferMemory);


			// operators
			operator VkDevice() {return device;}
			void operator<<(const char *extension) noexcept {requireExtension(extension);}
		
		private:
			Instance &instance;
			PhysicalDevice &physicalDevice;
			VkDevice device;

			std::vector<const char *> requiredExtensions;
			std::vector<float> queuePriorities;
			uint32_t queueCount = 0;

			std::vector<std::array<VkQueue, FAMILY_TYPE_COUNT>> queues;
	};
}