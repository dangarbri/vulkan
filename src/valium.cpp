#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <optional>
#include "valium.h"
#include "valium_queue.h"
#include "validation_layers.h"
#include "valium_device.h"

struct Valium::impl {
  /** Vulkan instance for use with vulkan APIs */
  VkInstance instance;
  /** Self */
  Valium* inst;
  /** Interface for interacting with the GPU. Set by @a selectDevice() */
  ValiumDevice* device = nullptr;
  /** Extensions to be requested on the instance */
  std::vector<const char*> requestedExtensions;

  /** Creates the vulkan instance and assigns it to instance */
  void initVulkanInstance(const char* app_name);

  /** Checks the vulkan API for a list of available extensions */
  std::vector<VkExtensionProperties> getVulkanExtensions();

  /**
   * @brief Applies instance extensions.
   * This function will set VK_KHR_get_physical_device_properties2 if it is
   * available since it is a dependency of VK_KHR_portability_subset. And
   * VK_KHR_portability_subset MUST be set (per the spec) if it is an available
   * extension, therefore VK_KHR_get_physical_device_properties2 must also be set.
   *
   * This function also sets the extensions required by GLFW
   *
   * This function may be extended to include other extensions.
   *
   * @param[out] info CreateInfo data to place extensions into.
   */
  void SetInstanceExtensions(VkInstanceCreateInfo &info);

  /** Verifies that the glfw extensions exist in vulkan's available extensions */
  bool verifyGlfwWorksWithVulkan();

  /** Selects a GPU to use for rendering */
  void selectDevice();

  /** Checks if a GPU is suitable for rendering */
  bool isDeviceSuitable(VkPhysicalDevice device);
};

Valium::Valium(const char* app_name) {
  _impl = new impl();
  _impl->inst = this;
  _impl->initVulkanInstance(app_name);
  _impl->selectDevice();
}

Valium::~Valium() {
  if (_impl->device != nullptr) {
    delete _impl->device;
  }
  
  vkDestroyInstance(_impl->instance, nullptr);
  std::cout << "Destroyed vulkan instance" << std::endl;

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
  ValidationLayers::enableValidationLayers(&createInfo);
#endif

  // Update createInfo with desired extensions
  SetInstanceExtensions(createInfo);

  if (verifyGlfwWorksWithVulkan() == false) {
     throw std::runtime_error("This Vulkan API does not support GLFW");
  }
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
 }

std::vector<VkExtensionProperties> Valium::impl::getVulkanExtensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
  return extensions;
}

void Valium::impl::SetInstanceExtensions(VkInstanceCreateInfo &info) {
  // Get the available extensions
  std::vector<std::string> availableExtensions = inst->GetAvailableExtensions();
  // Check if the available extensions contains VK_KHR_get_physical_device_properties2
  for (auto ext : availableExtensions) {
    if (ext == "VK_KHR_get_physical_device_properties2") {
      requestedExtensions.push_back("VK_KHR_get_physical_device_properties2");
    }
  }

  // Push the GLFW extensions into the list
  uint32_t glfwExtensionCount;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  for (uint32_t i = 0; i < glfwExtensionCount; i++) {
    requestedExtensions.push_back(glfwExtensions[i]);
  }

  info.enabledExtensionCount = requestedExtensions.size();
  info.ppEnabledExtensionNames = requestedExtensions.data();
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

void Valium::impl::selectDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;

  // TODO: Rate installed devices and select the one with the most
  // appropriate features
  for (VkPhysicalDevice device : devices) {
    if (isDeviceSuitable(device)) {
      if (selectedDevice == VK_NULL_HANDLE) {
#ifndef NDEBUG
        std::cout << "Choosing device " << device << std::endl;
#endif
        selectedDevice = device;
      }
    }
  }

  if (selectedDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }

  // Now that a device has been selected, wrap it with some valium.
  device = new ValiumDevice(selectedDevice);
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
  QueueFamilyIndices indices = ValiumQueue::GetQueueIndices(device);

  // No particular features must be specified, but you could return false
  // if a certain feature isn't supported.
  return indices.isComplete();
}
