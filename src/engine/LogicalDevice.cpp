#include "engine/LogicalDevice.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace vk_engine{
	LogicalDevice::LogicalDevice(Instance &instance, PhysicalDevice &device) : instance{instance}, physicalDevice{device}{}

	LogicalDevice::~LogicalDevice(){
		vkDestroyDevice(device, nullptr);
	}

	void LogicalDevice::build(){
		std::vector<PhysicalDevice::FamilyDetails> families = physicalDevice.getFamilies();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		// make a unique verison of each families
		std::vector<PhysicalDevice::FamilyDetails> uniqueFamilies;

		for (const auto &family : families){
			bool isUnique = true;
			for (const auto &uniqueFamily : uniqueFamilies){
				if (uniqueFamily.family == family.family){
					isUnique = false;
					break;
				}
			}

			if (isUnique) uniqueFamilies.push_back(family);
		}
		
		for (auto &queueFamily : uniqueFamilies){
			assert((queueFamily.family == VK_NULL_HANDLE || queueFamily.type == FAMILY_NONE) && "cannot use a non initialized queue family");

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily.family;
			queueCreateInfo.queueCount = queueCount;
			queueCreateInfo.pQueuePriorities = queuePriorities.data();
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures features = physicalDevice.getRequiredFeaturesStruct();

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &features;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		if (instance.isValidationLayerEnable()){
			createInfo.enabledLayerCount = static_cast<uint32_t>(instance.getValidationLayers().size());
			createInfo.ppEnabledLayerNames = instance.getValidationLayers().data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
			throw std::runtime_error("failed to create logical device");
		
		queues.resize(queueCount);
		for (int i=0; i<static_cast<int>(queueCount); i++){
			// set all of the queues to nullptr
			queues[i].fill(nullptr);

			// set the used queues
			for (const auto &family : families){
				vkGetDeviceQueue(device, family.family, i, &queues[i][family.type]);
			}
		}
	}

	void LogicalDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,VkDeviceMemory &bufferMemory){

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = physicalDevice.findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void LogicalDevice::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) {
		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = physicalDevice.findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		if (vkBindImageMemory(device, image, imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to bind image memory!");
		}
	}
}