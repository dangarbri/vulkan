#include "valium_device.h"
#include "valium_queue.h"
#include "validation_layers.h"
#include <vector>
#include <iostream>
#include <string>
#include <set>

/**
 * Private functions for ValiumDevice
 */
struct ValiumDevice::ValiumDeviceImpl {
  /** Represents the physical device for this @a ValiumDevice */
  const VkPhysicalDevice physicalDevice;
  /** Surface that will be rendered to */
  const VkSurfaceKHR surface;

  /** Logical Device to be used with the vulkan API */
  VkDevice device;

  /** Queue descriptor for interfacing with the GPU's command queue */
  VkQueue graphicsQueue;

  /**
   * @brief Queue that manages rendering contents to the window.
   * Initialized with CreateLogicalDevice()
   */
  VkQueue presentQueue;

  /** Extensions to enable on the device */
  std::vector<const char*> desiredExtensions;

  /** Constructs and assigns the constant device */
  ValiumDeviceImpl(const VkPhysicalDevice d, const VkSurfaceKHR surface) : physicalDevice(d), surface(surface) {}

  /** Creates the logical device around @a ValiumDeviceImpl::device */
  void CreateLogicalDevice();

  /** Sets the extensions to be used when creating the logical device with CreateLogicalDevice()
   * @param[out] createInfo The struct to place the extensions into.
   */
  void SetExtensions(VkDeviceCreateInfo& createInfo);

  /**
   * Initializes ValiumDeviceImpl::presentQueue.
   * Called by CreateLogicalDevice()
   *
   * @param[in] indices QueueFamilyIndices object containing this device's queue info
   * @param[out] queues Vector to be updated with desired queue results
   * @param[in] priority Priority to use for all queues
   */
  void GetDesiredQueues(QueueFamilyIndices indices, std::vector<VkDeviceQueueCreateInfo> &queues, float* priority);
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
  // All information for device creation goes into this struct.
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  
  // Get the desired queues to be created with the device
  QueueFamilyIndices indices = ValiumQueue::GetQueueIndices(physicalDevice, surface);
  std::vector<VkDeviceQueueCreateInfo> desiredQueues;
  float priority = 1.0f;
  GetDesiredQueues(indices, desiredQueues, &priority);
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(desiredQueues.size());
  createInfo.pQueueCreateInfos = desiredQueues.data();

  // Currently don't use any special device features
  VkPhysicalDeviceFeatures deviceFeatures{};
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

  // Retrieve queues
  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
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

void ValiumDevice::ValiumDeviceImpl::GetDesiredQueues(QueueFamilyIndices indices, std::vector<VkDeviceQueueCreateInfo> &desiredQueues, float* priority) {
  // Place the queue families into a set (in case they're the same
  // queue index, we should only create queue once).
  std::set<uint32_t> uniqueQueueFamilies = {
    indices.graphicsFamily.value(),
    indices.presentFamily.value()
  };

  // Create the queue creation structs and add them to the
  // desired queues
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
#ifdef SHOW_QUEUE_CREATION
    std::cout << "Attempting to create queue " << queueFamily << std::endl;
#endif
    VkDeviceQueueCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    createInfo.queueFamilyIndex = queueFamily;
    createInfo.queueCount = 1;
    createInfo.pQueuePriorities = priority;
    desiredQueues.push_back(createInfo);
  }
}
