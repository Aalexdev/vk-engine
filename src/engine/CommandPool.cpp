#include "engine/CommandPool.hpp"

// std
#include <stdexcept>

namespace vk_engine{
	CommandPool::CommandPool(LogicalDevice &device) : device{device}{}
	CommandPool::~CommandPool(){
		vkDestroyCommandPool(device, commandPool, nullptr);
	}

	void CommandPool::build(PhysicalDevice &physicalDevice){
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		createInfo.queueFamilyIndex = physicalDevice.getFamilies()[family].family;
		createInfo.flags = flags;

		if (vkCreateCommandPool(device, &createInfo, nullptr, &commandPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create command buffer");
	}
}