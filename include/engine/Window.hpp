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

			/**
			 * @brief get if the window has been resized
			 * @return true if resized, false if not
			 */
			bool resized() const noexcept {return frameBufferResized;}

			/**
			 * @brief get the size of the window as a vulkan extent
			 * @return VkExtent2D 
			 */
	        VkExtent2D getExtent() const noexcept {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}

			/**
			 * @brief reset the resized flag of the window
			 */
			void resetWindowResizedFlag() noexcept {frameBufferResized = false;}

			// operators
			operator GLFWwindow*() const noexcept {return window;}

		private:
			void createWindow(const std::string &title, const int &width, const int &height);
	        static void frameBufferResizedCallback(GLFWwindow *window, int width, int height);


			GLFWwindow *window;
        	bool frameBufferResized = false;
			int width, height;
	};
}