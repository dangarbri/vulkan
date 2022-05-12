#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <optional>
#include "valium.h"

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  bool isComplete() {
    return graphicsFamily.has_value();
  }
};

struct Valium::impl {
  VkInstance instance;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
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

  // Selects a GPU to use for rendering
  void selectPhysicalDevice();

  // Checks if a GPU is suitable for rendering
  bool isDeviceSuitable(VkPhysicalDevice device);

  // Finds the queue family to use for the pipeline
  QueueFamilyIndices findGraphicsQueue(VkPhysicalDevice device);

#ifndef NDEBUG
  const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

  // Enables validation layers for debugging vulkan usage. To be
  // called when initializing the vulkan instance
  void enableValidationLayers(VkInstanceCreateInfo* info);

  // Checks that the given validation layers are supported by the system
  bool checkValidationLayerSupport(std::vector<const char*> validationLayers);
#endif
};

Valium::Valium(const char* app_name) {
  _impl = new impl();
  _impl->inst = this;
  _impl->initVulkanInstance(app_name);
  _impl->selectPhysicalDevice();
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

#ifndef NDEBUG
  enableValidationLayers(&createInfo);
#endif

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

void Valium::impl::selectPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // TODO: Rate installed devices and select the one with the most
  // appropriate features
  for (VkPhysicalDevice device : devices) {
    if (isDeviceSuitable(device)) {
      if (physicalDevice == VK_NULL_HANDLE) {
#ifndef NDEBUG
        std::cout << "Choosing device " << device << std::endl;
#endif
        physicalDevice = device;
      }
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool Valium::impl::isDeviceSuitable(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties props;
  vkGetPhysicalDeviceProperties(device, &props);

#ifndef NDEBUG
  std::cout << "Found device [" << device << "]: " << props.deviceName << std::endl;
#endif

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(device, &features);

  // Make sure there is at least one queue that supports graphics.
  QueueFamilyIndices indices = findGraphicsQueue(device);

  // No particular features must be specified, but you could return false
  // if a certain feature isn't supported.
  return indices.isComplete();
}

QueueFamilyIndices Valium::impl::findGraphicsQueue(VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  // Logic to find queue family indices to populate struct with
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }
  return indices;
}

#ifndef NDEBUG
void Valium::impl::enableValidationLayers(VkInstanceCreateInfo* info) {
  if (!checkValidationLayerSupport(validationLayers)) {
    throw std::runtime_error("The specified validation layers are not supported");
  }

  std::cout << "Enabling validation layers" << std::endl;
  info->enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
  info->ppEnabledLayerNames = validationLayers.data();
}

bool Valium::impl::checkValidationLayerSupport(const std::vector<const char*> validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        std::cout << "Validation layer " << layerName << " is not supported." << std::endl;
        return false;
      }
    }

    return true;
}
#endif
