#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include "window.h"
#include "valium.h"

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  Window* _window;
  Valium* _valium;
  
  void initWindow() {
    // Create the window
    _window = new Window("Vulkan");
  }
  
  void initVulkan() {
    _valium = new Valium("Vulkan");
    std::vector<std::string> names = _valium->GetAvailableExtensions();
    std::cout << "available extensions:" << std::endl;
    for (auto name : names) {
      std::cout << "\t" << name << std::endl;
    }
  }

  void mainLoop() {
    GLFWwindow* win = _window->GetWindow();
    while (!glfwWindowShouldClose(win)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    delete _valium;
    delete _window;
  }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
