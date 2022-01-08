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
			assert(queueFamily.family == VK_NULL_HANDLE || queueFamily.type == PhysicalDevice::NONE_FAMILY && "cannot use a non initialized queue family");

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
		
	}
}