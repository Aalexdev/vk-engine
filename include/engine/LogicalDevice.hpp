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
	};
}