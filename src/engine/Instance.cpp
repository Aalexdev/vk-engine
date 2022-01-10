#include "engine/Instance.hpp"

// libs
#include <GLFW/glfw3.h>

// std
#include <stdexcept>
#include <cstring>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <iterator>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) {

    return VK_FALSE;
}

static inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator){

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace vk_engine{
	Instance::Instance(Window &window) : window{window}{}
	Instance::Instance(Window &window, const bool &validationLayerEnable) : window{window}, validationLayerEnable{validationLayerEnable}{}

	Instance::~Instance(){
		if (validationLayerEnable) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

		vkDestroyInstance(instance, nullptr);
	}

	void Instance::enableValidationLayer() noexcept{
		validationLayerEnable = true;
		validationLayers.push_back("VK_LAYER_KHRONOS_validation");
	}

	void Instance::build(){
		if (validationLayerEnable)
			checkValidationLayers();

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = applicationName.c_str();
		appInfo.applicationVersion = applicationVersion;
		appInfo.pEngineName = engineName.c_str();
		appInfo.engineVersion = engineVersion;
		appInfo.apiVersion = apiVersion;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (validationLayerEnable) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			setDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
		} else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			throw std::runtime_error("failed to create instance!");
		
		hasGflwRequiredInstanceExtensions();
		setupDebugMessenger();

		// and create the surface
		window.createSurface(instance, &surface);
	}

	void Instance::checkValidationLayers(){

		// get the layer count and place them into a vector
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// for each requiered validation layers, check if it's supported
		for (const char* layerName : validationLayers){
			bool layerFound = false;

			for (const auto &layerProperties : availableLayers){
				if (strcmp(layerProperties.layerName, layerName) == 0){
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				throw std::runtime_error("cannot found \"" + std::string(layerName) + "\" validation layer");
			
		}
	}

	std::vector<const char*> Instance::getRequiredExtensions() noexcept{
		uint32_t glfwExtentionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtentionCount);

		if (validationLayerEnable)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		
		return extensions;
	}

	void Instance::setDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) const noexcept{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
	}

	void Instance::hasGflwRequiredInstanceExtensions() {

		// get extension 
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// push them into an available set
		std::unordered_set<std::string> available;
		for (const auto &extension : extensions) {
			available.insert(extension.extensionName);
		}

		// get the required extension
		auto requiredExtensions = getRequiredExtensions();
		
		// check if required extension are available
		for (const auto &required : requiredExtensions) {
			if (available.find(required) == available.end()) {
				throw std::runtime_error("Missing required glfw extension");
			}
		}

		// copy the required extentions
		for (const char *extension : requiredExtensions)
			this->requiredExtensions.push_back(std::string(extension));
		
		// copy the available extentions
		for (auto &extension : available)
			availableExtensions.push_back(extension);	
	}

	void Instance::setupDebugMessenger(){
		if (!validationLayerEnable) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		setDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			throw std::runtime_error("failed to create debug uitls messenger");
	}
}