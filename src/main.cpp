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
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  Valium* _valium;
  
  void initVulkan() {
    _valium = new Valium("Vulkan");
#ifdef SHOW_AVAILABLE_EXTENSIONS
    std::vector<std::string> names = _valium->GetAvailableExtensions();
    std::cout << "available extensions:" << std::endl;
    for (auto name : names) {
      std::cout << "\t" << name << std::endl;
    }
#endif
  }

  void mainLoop() {
    GLFWwindow* win = _valium->GetWindow();
    while (!glfwWindowShouldClose(win)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    delete _valium;
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
