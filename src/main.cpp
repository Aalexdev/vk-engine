#include <iostream>

// files
#include "engine/Window.hpp"
#include "engine/Instance.hpp"
#include "engine/PhysicalDevice.hpp"
#include "engine/LogicalDevice.hpp"
#include "engine/CommandPool.hpp"

int main(int argc, char **argv){
	vk_engine::Window window("title", 1080, 720);

	vk_engine::Instance instance;
	instance.enableValidationLayer();
	instance.setApiVersion(vk_engine::Instance::API_VERSION_1_2);
	instance.build(window);

	vk_engine::PhysicalDevice physicalDevice(instance);
	physicalDevice.requireFamily(vk_engine::PhysicalDevice::GRAPHIC_FAMILY);
	physicalDevice.requireFamily(vk_engine::PhysicalDevice::PRESENT_FAMILY);
	physicalDevice.requireFeature(vk_engine::PhysicalDevice::FEATURE_SAMPLER_ANISOTROPY);
	physicalDevice.build();

	vk_engine::LogicalDevice logicalDevice(instance, physicalDevice);
	logicalDevice.requireExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	logicalDevice.setQueueCount(1);
	logicalDevice.setQueuePriority(1.0f, 0);
	logicalDevice.build();

	vk_engine::CommandPool commandPool(logicalDevice);
	commandPool.setFamily(vk_engine::PhysicalDevice::GRAPHIC_FAMILY);
	commandPool.setFlags(vk_engine::CommandPool::FLAG_TRANSIENT | vk_engine::CommandPool::FLAG_RESET_BUFFER);
	commandPool.build(physicalDevice);

	while (!window.shouldClose()){
		glfwPollEvents();
		_sleep(0.016);
	}	
}