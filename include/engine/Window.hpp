#pragma once

// std
#include <string>

// libs
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace vk_engine{
	class Window{
		public:
			Window(const std::string &title, const int &width, const int &height);
			~Window();

			// delete copy
			Window(const Window &) = delete;
			Window &operator=(const Window &) = delete;

			// get window
			/**
			 * @brief get the glfw window instance
			 * @warning do not copy until you not sure that the pointer will be released before the window
			 * @return GLFWwindow* 
			 */
			GLFWwindow* getWindow() const noexcept {return window;}

			/**
			 * @brief return true if the window should close, false if not
			 * @attention assert is used in this function to avoid to send a NULL window to glfw
			 * @return true on exit event like alt+F4 or by the close button
			 */
			bool shouldClose() const noexcept;

			/**
			 * @brief create the surface from the vulkan instance
			 * @param instance the vulkan instance
			 * @param surface a pointer to the surface
			 */
			void createSurface(VkInstance instance, VkSurfaceKHR *surface);

			// operators
			operator GLFWwindow*() const noexcept {return window;}

		private:
			void createWindow(const std::string &title, const int &width, const int &height);

			GLFWwindow *window;
	};
}