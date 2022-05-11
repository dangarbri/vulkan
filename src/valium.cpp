#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "valium.h"

struct Valium::impl {
  VkInstance instance;
  Valium* inst;

  // Creates the vulkan instance and assigns it to instance
  void initVulkanInstance(const char* app_name);

  // Gets the required glfw extensions to pass to vulkan and adds
  // them to the given info struct.
  void setGlfwExtensions(VkInstanceCreateInfo* info);

  // Checks the vulkan API for a list of available extensions
  std::vector<VkExtensionProperties> getVulkanExtensions();

  // Verifies that the glfw extensions exist in vulkan's available extensions
  bool verifyGlfwWorksWithVulkan();
};

Valium::Valium(const char* app_name) {
  _impl = new impl();
  _impl->inst = this;
  _impl->initVulkanInstance(app_name);
}

Valium::~Valium() {
  std::cout << "Destroyed vulkan instance" << std::endl;
  vkDestroyInstance(_impl->instance, nullptr);
  delete _impl;
}

std::vector<std::string> Valium::GetAvailableExtensions() {
  std::vector<VkExtensionProperties> extensions = _impl->getVulkanExtensions();
  std::vector<std::string> extension_names;

  for (auto ext : extensions) {
    extension_names.push_back(ext.extensionName);
  }

  return extension_names;
}

void Valium::impl::initVulkanInstance(const char* app_name) {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = app_name;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledLayerCount = 0;

  // Load GLFW extensions into the info struct
  setGlfwExtensions(&createInfo);

  if (verifyGlfwWorksWithVulkan() == false) {
     throw std::runtime_error("This Vulkan API does not support GLFW");
  }
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
 }

void Valium::impl::setGlfwExtensions(VkInstanceCreateInfo* info) {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  info->enabledExtensionCount = glfwExtensionCount;
  info->ppEnabledExtensionNames = glfwExtensions;
}

std::vector<VkExtensionProperties> Valium::impl::getVulkanExtensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
  return extensions;
}

bool Valium::impl::verifyGlfwWorksWithVulkan() {
  // Get extensions required by glfw
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  // Get extensions available from vulkan
  std::vector<std::string> extensions = inst->GetAvailableExtensions();

  // Verify that each extension exists in the vector
  for (int idx = 0; idx < glfwExtensionCount; idx++) {
    std::cout << "Checking for " << glfwExtensions[idx] << " in vulkan extensions" << std::endl;
    auto result = std::find(extensions.begin(), extensions.end(), std::string(glfwExtensions[idx]));
    if (result == extensions.end()) {
      std::cout << "Required extension " << glfwExtensions[idx] << " is not available in vulkan" << std::endl;
      return false;
    }
  }

  return true;
}
