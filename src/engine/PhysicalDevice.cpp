#include "engine/PhysicalDevice.hpp"

// std
#include <stdexcept>
#include <set>

namespace vk_engine{
	PhysicalDevice::PhysicalDevice(Instance &instance) : instance{instance}{}

	PhysicalDevice::~PhysicalDevice(){}

	void PhysicalDevice::build(){
		// query physical device count
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
			throw std::runtime_error("failed to find GPUs with vulkan support");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (auto &device : devices){
			if (isSuitableDevice(device)){
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("failed to found a suitable GPU");

		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	}

	bool PhysicalDevice::isSuitableDevice(VkPhysicalDevice device){
		auto families = getFamilies(device);
		if (!checkDeviceExtensions(device)) return false;

		swapChainSupport = getSwapChainSupport(device);
		bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return swapChainAdequate && checkFeatures(supportedFeatures);
	}

	std::array<uint32_t, PhysicalDevice::FAMILY_TYPE_COUNT> PhysicalDevice::getFamilies(VkPhysicalDevice physicalDevice){
		std::bitset<FAMILY_TYPE_COUNT> availableFamilies;
		std::array<uint32_t, PhysicalDevice::FAMILY_TYPE_COUNT> families;

		// query availables queues
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		uint32_t i=0;
		for (const auto &queueFamily : queueFamilies){
			if (queueFamily.queueCount == 0) continue;

			// graphic queue
			if (requiredFamilies[GRAPHIC_FAMILY] && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
				families[GRAPHIC_FAMILY] = i;
				availableFamilies.set(GRAPHIC_FAMILY);
				this->families.push_back({i, GRAPHIC_FAMILY});
			}

			// compute queue
			if (requiredFamilies[COMPUTE_FAMILY] && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT){
				families[COMPUTE_FAMILY] = i;
				availableFamilies.set(COMPUTE_FAMILY);
				this->families.push_back({i, COMPUTE_FAMILY});
			}

			// transfer queue
			if (requiredFamilies[TRANSFER_FAMILY] && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT){
				families[TRANSFER_FAMILY] = i;
				availableFamilies.set(TRANSFER_FAMILY);
				this->families.push_back({i, TRANSFER_FAMILY});
			}

			// protected queue
			if (requiredFamilies[PROTECTED_FAMILY] && queueFamily.queueFlags & VK_QUEUE_PROTECTED_BIT){
				families[PROTECTED_FAMILY] = i;
				availableFamilies.set(PROTECTED_FAMILY);
				this->families.push_back({i, PROTECTED_FAMILY});
			}

			// spars queue
			if (requiredFamilies[SPARSE_BINDING_FAMILY] && queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT){
				families[SPARSE_BINDING_FAMILY] = i;
				availableFamilies.set(SPARSE_BINDING_FAMILY);
				this->families.push_back({i, SPARSE_BINDING_FAMILY});
			}

			// present
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, instance.getSurface(), &presentSupport);
			if (requiredFamilies[PRESENT_FAMILY] && presentSupport){
				families[PRESENT_FAMILY] = i;
				availableFamilies.set(PRESENT_FAMILY);
				this->families.push_back({i, PRESENT_FAMILY});
			}

			if (availableFamilies == requiredFamilies) break;
			i++;
		}


		if (availableFamilies != requiredFamilies)
			throw std::runtime_error("failed to find the wanted queues");
		
		return families;
	}

	bool PhysicalDevice::checkDeviceExtensions(VkPhysicalDevice device){
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(this->requiredExtensions.begin(), this->requiredExtensions.end());

		for (const auto &extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);
		
		return requiredExtensions.empty();
	}

	PhysicalDevice::SwapChainSupport PhysicalDevice::getSwapChainSupport(VkPhysicalDevice device){
		SwapChainSupport details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, instance.getSurface(), &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance.getSurface(), &formatCount, nullptr);

		if (formatCount != 0){
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance.getSurface(), &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance.getSurface(), &presentModeCount, nullptr);

		if (presentModeCount != 0){
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance.getSurface(), &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool PhysicalDevice::checkFeatures(VkPhysicalDeviceFeatures features){
		// convert the struct into an array
		VkBool32* arr = reinterpret_cast<VkBool32*>(&features);

		for (int i=0; i<FEATURES_COUNT; i++){
			// if the feature is required but not available
			if (requiredFeatures[i] && !arr[i]){
				return false;
			}
		}
		return true;
	}

	VkPhysicalDeviceFeatures PhysicalDevice::getRequiredFeaturesStruct() const noexcept{
		VkPhysicalDeviceFeatures features;
		VkBool32* arr = reinterpret_cast<VkBool32*>(&features);

		for (int i=0; i<FEATURES_COUNT; i++){
			arr[i] = requiredFeatures[i];
		}

		return features;
	}

	PhysicalDevice::FamilyDetails PhysicalDevice::getFamily(Family family) const{
		for (auto &f : families){
			if (f.type == family) return f;
		}
		throw std::runtime_error("failed to found the given family");
	}

	VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			} else if (
					tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type");
	}
}