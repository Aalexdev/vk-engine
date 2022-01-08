#pragma once

#include "engine/Window.hpp"

// libs
#include <vulkan/vulkan.h>

// std
#include <vector>
#include <string>

namespace vk_engine{
	class Instance{
		public:
			static constexpr uint32_t API_VERSION_1_0 = VK_API_VERSION_1_0;
			static constexpr uint32_t API_VERSION_1_1 = VK_API_VERSION_1_1;
			static constexpr uint32_t API_VERSION_1_2 = VK_API_VERSION_1_2;

			Instance();
			Instance(const bool &validationLayerEnable);
			~Instance();

			// delete copy
			Instance(const Instance &) = delete;
			Instance &operator=(const Instance &) = delete;

			/**
			 * @brief get the VkInstance
			 * @warning do not copy until you not sure that the pointer will be released before the instance
			 * @return VkInstance 
			 */
			VkInstance getInstance() const noexcept {return instance;}

			/**
			 * @brief activate the vulkan validation layer
			 */
			void enableValidationLayer() noexcept;

			/**
			 * @brief create the instance from the given extensions and parameters
			 */
			void build(Window &window);

			/**
			 * @brief set the application name
			 * @param name the name of the application
			 */
			void setApplicationName(const std::string &name) noexcept {applicationName = name;}

			/**
			 * @brief set the engine name
			 * @param name the name of the engine
			 */
			void setEngineName(const std::string &name) noexcept {engineName = name;}

			void setApplicationVersion(const float &major, const float &minor, const float &patch) {applicationVersion = VK_MAKE_VERSION(major, minor, patch);}
			void setEngineVersion(const float &major, const float &minor, const float &patch){engineVersion = VK_MAKE_VERSION(major, minor, patch);}
			void setApiVersion(const float &variant, const float &major, const float &minor, const float &patch){apiVersion = VK_MAKE_API_VERSION(variant, major, minor, patch);}

			void setApplicationVersion(uint32_t version) {applicationVersion = version;}
			void setEngineVersion(uint32_t version) {engineVersion = version;}
			void setApiVersion(uint32_t version) {apiVersion = version;}

			/**
			 * @brief get the surface of the instance with the window
			 * @return VkSurfaceKHR 
			 */
			VkSurfaceKHR getSurface() const noexcept {return surface;}

			/**
			 * @brief get if the valisation layers has been required
			 */
			bool isValidationLayerEnable() const noexcept {return validationLayerEnable;}

			/**
			 * @brief get the requires validation layers 
			 * @return std::vector<const char*> 
			 */
			const std::vector<const char*> &getValidationLayers() noexcept {return validationLayers;}

			// operators
			operator VkInstance() const noexcept {return instance;}
		
		private:
			void checkValidationLayers();
			std::vector<const char*> getRequiredExtensions() noexcept;
			void setDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) const noexcept;
			void hasGflwRequiredInstanceExtensions();
			void setupDebugMessenger();

			VkInstance instance;
			VkDebugUtilsMessengerEXT debugMessenger;
			VkSurfaceKHR surface;

			std::vector<const char*> validationLayers;

			bool validationLayerEnable = false;

			std::string applicationName = "NoNameApp";
			std::string engineName = "NoEngine";
			uint32_t applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
			uint32_t apiVersion = VK_API_VERSION_1_0;

			std::vector<std::string> availableExtensions;
			std::vector<std::string> requiredExtensions;
	};
}