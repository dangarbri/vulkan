#pragma once

#include <vulkan/vulkan.h>
#include <optional>

/**
 * Contains information for a desired queue family's index in a physical device
 */
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  bool isComplete() {
    return graphicsFamily.has_value();
  }
};

/**
 * Manages operations for reading and handling queue features on a
 * physical device
 */
class ValiumQueue
{
 public:
  /**
   * Returns the queue family indices for the given @a device
   * @param[in] device The device to read queue information for.
   * @returns QueueFamilyIndices object containing indices of interest
   */
  static QueueFamilyIndices GetQueueIndices(const VkPhysicalDevice device);
};
