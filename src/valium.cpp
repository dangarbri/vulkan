#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <optional>
#include <memory>
#include "valium.h"
#include "valium_queue.h"
#include "validation_layers.h"
#include "valium_device.h"
#include "valium_swapchain.h"
#include "window.h"

struct Valium::impl {
  /** Name of the running app */
  const char* app_name;
  /** Vulkan instance for use with vulkan APIs */
  VkInstance instance;
  /** Self */
  Valium* inst;
  /** Interface for interacting with the GPU. Set by @a selectDevice() */
  ValiumDevice* device = nullptr;
  /** Extensions to be requested on the instance */
  std::vector<const char*> requestedExtensions;
  /** Surface for rendering to */
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  /** Window and its operations */
  std::unique_ptr<Window> window;

  /** Creates the vulkan instance and assigns it to instance */
  void initVulkanInstance(const char* app_name);

  /** Checks the vulkan API for a list of available extensions */
  std::vector<VkExtensionProperties> getVulkanExtensions();

  impl(const char* name) : app_name(name) {}

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

  /** Creates the surface and saves it to @a surface */
  void CreateSurface();

  /** Creates the window using GLFW */
  void CreateWindow();
};

Valium::Valium(const char* app_name) {
  _impl = new impl(app_name);
  _impl->inst = this;
  _impl->CreateWindow();
  _impl->initVulkanInstance(app_name);
  _impl->CreateSurface();
  _impl->selectDevice();
}

Valium::~Valium() {
  if (_impl->device != nullptr) {
    delete _impl->device;
  }

  if (_impl->surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(_impl->instance, _impl->surface, nullptr);
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

GLFWwindow* Valium::GetWindow() {
  return _impl->window->GetWindow();
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

#ifdef SHOW_AVAILABLE_EXTENSIONS
  std::cout << "Enabling extensions." << std::endl;
  for (auto ext : requestedExtensions) {
    std::cout << "\t" << ext << std::endl;
  }
#endif

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
#ifdef SHOW_AVAILABLE_EXTENSIONS
    std::cout << "Checking for " << glfwExtensions[idx] << " in vulkan extensions" << std::endl;
#endif
    auto result = std::find(extensions.begin(), extensions.end(), std::string(glfwExtensions[idx]));
    if (result == extensions.end()) {
#ifdef SHOW_AVAILABLE_EXTENSIONS
      std::cout << "Required extension " << glfwExtensions[idx] << " is not available in vulkan" << std::endl;
#endif
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
  int width, height;
  glfwGetFramebufferSize(window->GetWindow(), &width, &height);
  device = new ValiumDevice(selectedDevice, surface,
                            static_cast<uint32_t>(width),
                            static_cast<uint32_t>(height));
}

bool Valium::impl::isDeviceSuitable(VkPhysicalDevice device) {
#ifndef NDEBUG
  VkPhysicalDeviceProperties props;
  vkGetPhysicalDeviceProperties(device, &props);

  std::cout << "Found device [" << device << "]: " << props.deviceName << std::endl;
#endif

  bool supportsRequiredExtensions = ValiumDevice::SupportsRequiredExtensions(device);

  // Make sure there is at least one queue that supports graphics.
  QueueFamilyIndices indices = ValiumQueue::GetQueueIndices(device, surface);

  // Make sure the swapchain with the device and surface can be used.
  bool isSwapchainGood = ValiumSwapchain::SupportsDrawing(device, surface);

  // No particular features must be specified, but you could return false
  // if a certain feature isn't supported.
  return indices.isComplete() && supportsRequiredExtensions && isSwapchainGood;
}

void Valium::impl::CreateSurface() {
  if (glfwCreateWindowSurface(instance, window->GetWindow(), nullptr, &surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void Valium::impl::CreateWindow() {
  window = std::unique_ptr<Window>(new Window(app_name));
}
