#pragma once

#include <vulkan/vulkan.h>
#include <optional>

/**
 * Contains information for a desired queue family's index in a physical device
 */
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }

  bool hasGraphics() {
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
   * @param[in] surface The surface to use if checking for a presentation queue
   * @returns QueueFamilyIndices object containing indices of interest
   */
  static QueueFamilyIndices GetQueueIndices(const VkPhysicalDevice device, const VkSurfaceKHR surface);
};
