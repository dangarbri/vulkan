#include "valium_device.h"
#include "valium_queue.h"

struct ValiumDevice::ValiumDeviceImpl {
  /** Represents the physical device for this @a ValiumDevice */
  const VkPhysicalDevice device;

  /** Constructs and assigns the constant device */
  ValiumDeviceImpl(const VkPhysicalDevice d) : device(d) {}

  /** Creates the logical device around @a device */
  void CreateLogicalDevice();
};

ValiumDevice::ValiumDevice(const VkPhysicalDevice device) {
  _impl = new ValiumDeviceImpl(device);
  _impl->CreateLogicalDevice();
}

ValiumDevice::~ValiumDevice() {
  delete _impl;
}

void ValiumDevice::ValiumDeviceImpl::CreateLogicalDevice() {
  QueueFamilyIndices indices = ValiumQueue::GetQueueIndices(device);

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
}
