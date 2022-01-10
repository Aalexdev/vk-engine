#include "engine/Window.hpp"

// std
#include <stdexcept>
#include <cassert>

namespace vk_engine{
	Window::Window(const std::string &title, const int &width, const int &height){
		createWindow(title, width, height);
	}

	Window::~Window(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::createWindow(const std::string &title, const int &width, const int &height){
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		
		if (!window){
			throw std::runtime_error("failed to create glfw window");
		}

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
	}

	bool Window::shouldClose() const noexcept{
		assert(window && "cannot check a non initialized window");
		return glfwWindowShouldClose(window);
	}

	void Window::createSurface(VkInstance instance, VkSurfaceKHR *surface){
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("failed to create the window surface");
	}

	void Window::frameBufferResizedCallback(GLFWwindow *window, int width, int height){
		auto w = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		w->frameBufferResized = true;
		w->width = width;
		w->height = height;
	}
}