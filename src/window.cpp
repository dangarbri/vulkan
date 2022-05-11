#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.h"
#include "app_config.h"
#include <iostream>

struct Window::impl {
  GLFWwindow* window = nullptr;
};

Window::Window(const char* title) {
  _impl = new impl();
  
  glfwInit();
  // Don't enable OpenGL API so we can use vulkan
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // Don't make the window resizable for now.
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _impl->window = glfwCreateWindow(WIDTH, HEIGHT, title, nullptr, nullptr);
}

Window::~Window() {
  std::cout << "Destroyed window" << std::endl;
  glfwDestroyWindow(_impl->window);
  glfwTerminate();
  delete _impl;
}

GLFWwindow* Window::GetWindow() {
  return _impl->window;
}
