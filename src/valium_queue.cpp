#include "valium_queue.h"
#include <vector>

QueueFamilyIndices ValiumQueue::GetQueueIndices(const VkPhysicalDevice device) {
  QueueFamilyIndices indices;
  // Get the number of queues available
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  // Create a vector to hold the queues and read them into memory
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  // Iterate over the available queues and fill in the indices struct
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
