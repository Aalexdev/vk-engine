#include <iostream>
#include <chrono>

// files
#include "engine/Window.hpp"
#include "engine/Instance.hpp"
#include "engine/PhysicalDevice.hpp"
#include "engine/LogicalDevice.hpp"
#include "engine/CommandPool.hpp"
#include "engine/Renderer.hpp"
#include "engine/Image.hpp"
#include "engine/SingleTimeCommands.hpp"

int main(int argc, char **argv){

	vk_engine::Window window("title", 1080, 720);

	vk_engine::Instance instance(window);
	instance.enableValidationLayer();
	instance.setApiVersion(vk_engine::Instance::API_VERSION_1_2);
	instance.build();

	vk_engine::PhysicalDevice physicalDevice(instance);
	physicalDevice.requireFamily(vk_engine::FAMILY_GRAPHIC);
	physicalDevice.requireFamily(vk_engine::FAMILY_PRESENT);
	physicalDevice.requireFeature(vk_engine::PhysicalDevice::FEATURE_SAMPLER_ANISOTROPY);
	physicalDevice.build();

	vk_engine::LogicalDevice logicalDevice(instance, physicalDevice);
	logicalDevice.requireExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	logicalDevice.setQueueCount(1);
	logicalDevice.setQueuePriority(1.0f, 0);
	logicalDevice.build();

	vk_engine::CommandPool commandPool(logicalDevice);
	commandPool.setFamily(vk_engine::FAMILY_GRAPHIC);
	commandPool.setFlags(vk_engine::CommandPool::FLAG_TRANSIENT | vk_engine::CommandPool::FLAG_RESET_BUFFER);
	commandPool.build();

	vk_engine::Renderer renderer(logicalDevice, commandPool);
	renderer.getSwapChain().setRefreshType(vk_engine::REFRESH_IMMEDIATE_MODE); // V-sync
	renderer.setAutoUpdateViewportSize(true);
	renderer.setClearColor(0.05f, 0.05f, 0.05f, 0.05f);
	renderer.build();

	vk_engine::Image image(logicalDevice, commandPool, "res/vulkan-logo.png");
	image.setFormat(vk_engine::Image::FORMAT_RGBA);
	image.setSourceFormat(vk_engine::Image::FORMAT_RGBA);
	image.setFilter(vk_engine::Image::FILTER_NEARTEST);
	image.setNomalizedCoordonates(true);
	image.build();
	
	auto startTime = std::chrono::high_resolution_clock::now();
	float counter = 0.f;
	int fps = 0;

	while (!window.shouldClose()){
		glfwPollEvents();
		startTime = std::chrono::high_resolution_clock::now();
		if (auto commandBuffer = renderer.beginFrame()){
			renderer.beginSwapChainRenderPass(commandBuffer);


			renderer.endSwapChainRenderPass(commandBuffer);
			renderer.endFrame();
		}

		counter += std::chrono::duration<float, std::chrono::milliseconds::period>(std::chrono::high_resolution_clock::now() - startTime).count();

		if (counter >= 1000){
			std::cout << "fps : " << fps << std::endl;
			fps = 0;
			counter = 0;
		} else {
			fps++;
		}
	}

    vkDeviceWaitIdle(logicalDevice);
}