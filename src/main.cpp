#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include "window.h"

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
  
  void initWindow() {
    // Create the window
    _window = new Window("Vulkan");
  }
  
  void initVulkan() {

  }

  void mainLoop() {
    GLFWwindow* win = _window->GetWindow();
    while (!glfwWindowShouldClose(win)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
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
