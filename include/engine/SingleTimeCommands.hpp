#pragma once

#include "engine/CommandPool.hpp"
#include "engine/LogicalDevice.hpp"

// libs
#include <vulkan/vulkan.h>

namespace vk_engine{
	class SingleTimeCommands{
		public:
			SingleTimeCommands(CommandPool &commandPool, LogicalDevice &device, VkQueue &queue);
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
			VkQueue &queue;
	};
}