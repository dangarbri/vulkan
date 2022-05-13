#include "valium_device.h"
#include "valium_queue.h"
#include "validation_layers.h"
#include <vector>
#include <iostream>
#include <string>

struct ValiumDevice::ValiumDeviceImpl {
  /** Represents the physical device for this @a ValiumDevice */
  const VkPhysicalDevice physicalDevice;
  /** Surface that will be rendered to */
  const VkSurfaceKHR surface;

  /** Logical Device to be used with the vulkan API */
  VkDevice device;

  /** Queue descriptor for interfacing with the GPU's command queue */
  VkQueue graphicsQueue;

  /** Extensions to enable on the device */
  std::vector<const char*> desiredExtensions;

  /** Constructs and assigns the constant device */
  ValiumDeviceImpl(const VkPhysicalDevice d, const VkSurfaceKHR surface) : physicalDevice(d), surface(surface) {}

  /** Creates the logical device around @a device */
  void CreateLogicalDevice();

  /** Sets the extensions to be used when creating the logical device with CreateLogicalDevice()
   * @param[out] createInfo The struct to place the extensions into.
   */
  void SetExtensions(VkDeviceCreateInfo& createInfo);
};

ValiumDevice::ValiumDevice(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
  _impl = new ValiumDeviceImpl(physicalDevice, surface);
  _impl->CreateLogicalDevice();
#ifndef NDEBUG
  std::cout << "Created logical device" << std::endl;
#endif
}

ValiumDevice::~ValiumDevice() {
  vkDestroyDevice(_impl->device, nullptr);
  delete _impl;
}

void ValiumDevice::ValiumDeviceImpl::CreateLogicalDevice() {
  QueueFamilyIndices indices = ValiumQueue::GetQueueIndices(physicalDevice, surface);

  VkDeviceQueueCreateInfo queueCreateInfo{};
  // Specify that we would like to use the graphics queue
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;
  // Use the maximum priority since it's the only queue
  float priority = 1.0f;
  queueCreateInfo.pQueuePriorities = &priority;

  // Currently don't use any special device features
  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;

#ifndef NDEBUG
  createInfo.enabledLayerCount = ValidationLayers::validationLayers.size();
  createInfo.ppEnabledLayerNames = ValidationLayers::validationLayers.data();
#else
  createInfo.enabledLayerCount = 0;
#endif

  SetExtensions(createInfo);

  auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
  if (result != VK_SUCCESS) {
    std::cout << "VkCreateDevice: " << result << std::endl;
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

void ValiumDevice::ValiumDeviceImpl::SetExtensions(VkDeviceCreateInfo &createInfo) {
  uint32_t numExtensions;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &numExtensions, nullptr);

  std::vector<VkExtensionProperties> properties(numExtensions);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &numExtensions, properties.data());

#ifdef SHOW_AVAILABLE_EXTENSIONS
  std::cout << "Found properties: " << std::endl;
#endif
  for (VkExtensionProperties props : properties) {
#ifdef SHOW_AVAILABLE_EXTENSIONS
    std::cout << "\t" << props.extensionName << std::endl;
#endif
    if (std::string(props.extensionName) == "VK_KHR_portability_subset") {
      desiredExtensions.push_back("VK_KHR_portability_subset");
      // This extension is required by portability_subset per the documentation found here:
      // https://vulkan.lunarg.com/doc/view/1.3.211.0/mac/1.3-extensions/vkspec.html#VK_KHR_portability_subset
#ifdef SHOW_AVAILABLE_EXTENSIONS
      std::cout << "\t\t" << "Adding " << props.extensionName << " to desired extension list" << std::endl;
#endif
    }
  }

  std::cout << "Requested extensions: " << std::endl;
  for (auto ext : desiredExtensions) {
    std::cout << "\t" << ext << std::endl;
  }
  createInfo.enabledExtensionCount = desiredExtensions.size();
  createInfo.ppEnabledExtensionNames = desiredExtensions.data();
}
