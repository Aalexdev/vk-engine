#pragma once

#include "engine/LogicalDevice.hpp"
#include "engine/PhysicalDevice.hpp"

// std
#include <vulkan/vulkan.h>

namespace vk_engine{
	class CommandPool{
		public:
			CommandPool(LogicalDevice &device);
			~CommandPool();

			enum Flags{
				FLAG_TRANSIENT = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
				FLAG_RESET_BUFFER = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
				FLAG_PROTECTED = VK_COMMAND_POOL_CREATE_PROTECTED_BIT
			};

			/**
			 * @brief set the family 
			 * @param family the family
			 */
			void setFamily(PhysicalDevice::Family family) noexcept {this->family = family;}

			/**
			 * @brief set the command pool flags
			 * @param flags the flags CommandPool::Flags or VKCommandPoolCreateFlags
			 */
			void setFlags(int flags) {this->flags = flags;}

			/**
			 * @brief build the commandBuffer
			 */
			void build(PhysicalDevice &physicalDevice);

			// operators
			operator VkCommandPool() const noexcept {return commandPool;}
		
		private:
			LogicalDevice &device;
			VkCommandPool commandPool;

			PhysicalDevice::Family family;
			int flags;
	};
}